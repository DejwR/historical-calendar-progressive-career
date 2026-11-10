#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <vector>

#include <swofty/json.hpp>

namespace {
using swofty::json::Value;

Value parse(const std::string &text)
{
    swofty::json::Error error;
    auto value = swofty::json::parse(text, &error);
    if (!error.ok())
        throw std::runtime_error(error.message);
    return value;
}

void send(const Value &message)
{
    std::cout << swofty::json::dump(message) << std::endl;
}

Value installed_addons()
{
    send(Value::object({{"id", 1}, {"method", "addons"}, {"params", nullptr}}));
    std::string line;
    if (std::getline(std::cin, line)) {
        const auto reply = parse(line);
        if (reply["id"].as_int(-1) == 1 && reply["result"].is_array())
            return reply["result"];
    }
    throw std::runtime_error("Swofty Launcher did not return the active profile's addons.");
}

Value build(const Value &params, bool supports_addons)
{
    std::ifstream file("db/optional-tracks.json", std::ios::binary);
    if (!file)
        throw std::runtime_error("The Historical Calendar track configuration is missing.");
    const auto config = parse(std::string(std::istreambuf_iterator<char>(file), {}));
    const auto *tracks = config.if_array();
    if (!tracks)
        throw std::runtime_error("The Historical Calendar track configuration is invalid.");

    auto result = Value::object({{"ops", Value::array()}, {"warnings", Value::array()}});
    std::vector<const Value *> selected;
    for (const auto &track : *tracks) {
        const auto &option = params["options"][track["option"].as_string()];
        if (option.is_string() && option.as_string() == track["value"].as_string())
            selected.push_back(&track);
    }
    if (selected.empty())
        return result;
    if (!supports_addons)
        throw std::runtime_error("Update Swofty Launcher to verify optional track addons.");

    const auto addons = installed_addons();
    const auto &installed = *addons.if_array();
    auto &build_ops = *result["ops"].if_array();
    for (const auto *track : selected) {
        const auto id = (*track)["addon"].as_string();
        const auto name = std::string((*track)["name"].as_string());
        const auto addon = std::ranges::find_if(installed, [id](const Value &entry) {
            return entry["id"].as_string() == id;
        });
        if (addon == installed.end() || !(*addon)["enabled"].as_bool()) {
            const auto reason = addon == installed.end() ? " is not installed" : " is disabled in this profile";
            result["warnings"].push_back(name + ": " + std::string(id) + reason +
                                         ". The calendar will use the standard substitute.");
            continue;
        }
        const auto *ops = (*track)["ops"].if_array();
        if (!ops)
            throw std::runtime_error("Missing calendar operations for " + name);
        for (const auto &op : *ops) {
            // Both Sepang settings share one activity row and one set of labels.
            if (std::ranges::find(build_ops, op) == build_ops.end())
                build_ops.push_back(op);
        }
    }
    return result;
}
}

int main()
{
    bool supports_addons = false;
    std::string line;
    while (std::getline(std::cin, line)) {
        const auto request = parse(line);
        if (!request["id"].is_int())
            continue;
        auto reply = Value::object({{"id", request["id"]}});
        try {
            const auto method = request["method"].as_string();
            if (method == "hello") {
                if (const auto *calls = request["params"]["host_calls"].if_array()) {
                    supports_addons = std::ranges::any_of(*calls, [](const Value &call) {
                        return call.as_string() == "addons";
                    });
                }
                reply["result"] = Value::object({{"api", 1}});
            } else if (method == "build") {
                reply["result"] = build(request["params"], supports_addons);
            } else if (method == "summary") {
                reply["result"] = nullptr;
            } else {
                reply["error"] = "Unsupported Historical Calendar request";
            }
        } catch (const std::exception &error) {
            reply["error"] = error.what();
        }
        send(reply);
    }
}
