# Defold Project Directory extension
This is a small native extension that makes it so that you can get the project directory of a game
that is being run from the editor (where bundled resources are not available).

```lua
-- Will return nil when not running from the editor or the path to the project root otherwise
print(defoldprojectdir.get_project_directory())

-- Will return the path to the game's executable (or nil on unsupported platforms)
print(defoldprojectdir.get_executable_path())

-- The path separator ("/" on POSIX, "\\" on Windows) 
print(defoldprojectdir.PATH_SEP)
```
