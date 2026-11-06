extends Node
## Quest progression. This API is used by real interactions and the full-chain smoke test.
signal updated
signal escaped
signal player_caught
var running: bool = false
var won: bool = false
var elapsed: float = 0.0
var catches: int = 0
var block: int = 2
var checkpoint: Vector2 = Vector2(1280, 544)
var searched: Dictionary = {}
var read_notes: Dictionary = {}

func reset() -> void:
    Inventory.reset()
    running = true
    won = false
    elapsed = 0.0
    catches = 0
    block = 2
    checkpoint = Vector2(1280, 544)
    searched.clear()
    read_notes.clear()
    updated.emit()

func advance_time(delta: float) -> void:
    if running and not won:
        elapsed += delta

func enter_block(value: int) -> void:
    if value == block:
        return
    block = value
    checkpoint = [Vector2(800, 544), Vector2(1280, 544), Vector2(1696, 544)][block - 1]
    updated.emit()

func caught() -> void:
    catches += 1
    player_caught.emit()
    updated.emit()

func interact(id: String, code: String = "") -> String:
    if searched.has(id):
        return "empty"
    var item: String = ""
    match id:
        "west_key": item = "west_key"
        "east_key": item = "east_key"
        "glue": item = "glue"
        "library_drawer":
            if not Inventory.has_item("west_key"):
                return "locked_library_drawer"
            item = "piece_1"
        "locker":
            if code.is_empty():
                return "code"
            if code != "2410":
                return "wrong_code"
            item = "piece_2"
        "fridge":
            if not Inventory.has_item("east_key"):
                return "locked_fridge"
            item = "piece_3"
        "exit":
            if not Inventory.has_item("restored_id"):
                return "exit_locked"
            won = true
            running = false
            escaped.emit()
            return "escape"
        _:
            return "empty"
    searched[id] = true
    Inventory.add_item(item)
    updated.emit()
    return item

func combine() -> bool:
    if not Inventory.combine():
        return false
    updated.emit()
    return true

func objective() -> String:
    if Inventory.has_item("restored_id"):
        return "RETURN TO THE ATRIUM / scan your restored ID at the main exit."
    if Inventory.piece_count() == 3 and Inventory.has_item("glue"):
        return "RESTORE YOUR ID / TAB: select three pieces + glue, then Combine."
    if not Inventory.has_item("piece_1"):
        if Inventory.has_item("west_key"):
            return "WEST / Use the restroom key on the librarian desk drawer."
        return "WEST / Find the library drawer key in the west restroom."
    if not Inventory.has_item("piece_2"):
        return "HUB / West classroom whiteboard hints the coworking locker code."
    if not Inventory.has_item("piece_3"):
        return "EAST / Classroom 103 holds the canteen key. Check the fridge."
    return "EAST / Find the glue stick in Classroom 103."
