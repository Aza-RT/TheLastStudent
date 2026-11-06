extends Node
signal spoken(text: String)
var lines: Dictionary = {}
var seen: Dictionary = {}

func _ready() -> void:
    lines = JSON.parse_string(FileAccess.get_file_as_string("res://data/dialogue.json"))

func say(key: String, once: bool = false) -> void:
    if once and seen.has(key):
        return
    seen[key] = true
    spoken.emit(str(lines.get(key, key)))
