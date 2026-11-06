extends Node
## Inventory owns item storage and material selection; state changes use signals.
signal changed
var items: Array[String] = []
var selected: Array[String] = []
var definitions: Dictionary = {}
const MATERIALS: Array[String] = ["piece_1", "piece_2", "piece_3", "glue"]

func _ready() -> void:
    definitions = JSON.parse_string(FileAccess.get_file_as_string("res://data/items.json"))

func reset() -> void:
    items.clear()
    selected.clear()
    changed.emit()

func has_item(id: String) -> bool:
    return items.has(id)

func add_item(id: String) -> void:
    if not has_item(id):
        items.append(id)
        changed.emit()

func toggle_selection(id: String) -> void:
    if selected.has(id):
        selected.erase(id)
    elif MATERIALS.has(id) and has_item(id):
        selected.append(id)
    changed.emit()

func can_combine() -> bool:
    for id: String in MATERIALS:
        if not has_item(id) or not selected.has(id):
            return false
    return true

func combine() -> bool:
    if not can_combine():
        return false
    for id: String in MATERIALS:
        items.erase(id)
    selected.clear()
    add_item("restored_id")
    return true

func piece_count() -> int:
    if has_item("restored_id"):
        return 3
    var count: int = 0
    for id: String in ["piece_1", "piece_2", "piece_3"]:
        count += int(has_item(id))
    return count
