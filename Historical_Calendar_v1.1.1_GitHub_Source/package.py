import json
from pathlib import Path
import zipfile


root = Path(__file__).resolve().parent
addon = root / 'addon'
build = root / 'build'
manifest = json.loads((addon / 'swofty.json').read_text(encoding='utf-8'))
path = build / f'Historical_Calendar_for_Progressive_Career_v{manifest["version"]}.swoftyaddon'
with zipfile.ZipFile(path, 'w', zipfile.ZIP_DEFLATED) as archive:
    for file in sorted(addon.rglob('*')):
        if file.is_file():
            archive.write(file, file.relative_to(addon).as_posix())
    archive.write(build / 'dejw_public_calendar.dll', 'native/dejw_public_calendar.dll')
    archive.write(build / 'optional-tracks.exe', 'bin/optional-tracks.exe')
with zipfile.ZipFile(path) as archive:
    assert archive.testzip() is None
print(path)
