## 1.4.2

- Fixed excessive memory checks in the sprint override that could cause long freezes and watchdog timeouts when entering a session.
- Optimized sprint handling by caching track checks per session, including weekends that already have the correct format.
- Fixed new 2016 careers generating 45 rounds instead of the correct 21 when selecting Full Season.
- Improved sprint diagnostics by including circuit names in the logs.

**2016 calendar fix:** Start a new 2016 career and select **Full Season**. Existing saves with an incorrect 45-round calendar are not repaired automatically.
