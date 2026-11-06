"""Deterministic map and editable narrative data; coordinates are in 32px tiles."""
import json
from pathlib import Path
root=Path(__file__).resolve().parents[1]/'game'
def write(name,data):
    p=root/name;p.parent.mkdir(parents=True,exist_ok=True);p.write_text(json.dumps(data,indent=2),encoding='utf-8')
rooms=[
 {'name':'LIBRARY','block':1,'rect':[3,3,20,11]},
 {'name':'CLASSROOM 101','block':1,'rect':[3,21,9,11]},
 {'name':'RESTROOM / WEST','block':1,'rect':[15,21,8,11]},
 {'name':'COWORKING','block':2,'rect':[28,3,20,11]},
 {'name':'ATRIUM / MAIN EXIT','block':2,'rect':[28,21,20,11]},
 {'name':'CANTEEN','block':3,'rect':[54,3,21,11]},
 {'name':'CLASSROOM 103','block':3,'rect':[54,21,10,11]},
 {'name':'RESTROOM / EAST','block':3,'rect':[67,21,8,11]}]
doors=[[11,14,3,1],[7,19,2,2],[18,19,2,2],[36,14,3,1],[36,19,3,2],[64,14,3,1],[58,19,2,2],[70,19,2,2]]
objects=[]
def obj(id,name,pos,tile,action,note=''):
    objects.append(dict(id=id,name=name,pos=pos,tile=tile,action=action,note=note))
obj('west_key','Restroom stall',[20,29],15,'west_key')
obj('library_drawer','Librarian desk',[7,5],8,'piece_1')
obj('code_note','Deadline whiteboard',[5,24],7,'note','WEEK 2. ROOM 4. GROUP 1. ZERO extensions.\n\nThe coworking locker code follows the numbers above, in order.\nWhoever resets it to 0000 buys coffee for the whole group.')
obj('locker','Coworking locker',[44,5],10,'piece_2')
obj('east_key','Teaching desk',[56,24],8,'east_key')
obj('glue','Glue stick',[61,29],21,'glue')
obj('fridge','Canteen refrigerator',[72,5],11,'piece_3')
obj('exit','Main exit turnstiles',[38,30],12,'exit')
obj('library_note','Library rules',[19,4],7,'note','LIBRARY RULES\n1. Silence after 22:00.\n2. Return your books.\n3. If a book returns itself, leave it alone.\n\nDrawer key lost in the west restroom. Again.\nWi-Fi password: still not your student ID.')
obj('menu_note','Canteen menu',[56,5],7,'note','NIGHT MENU\nCoffee: unavailable.\nSoup: yesterday.\nDeadline extensions: sold out.\n\nPlease lock the refrigerator. The key belongs in Classroom 103.')
obj('glue_note','Lab safety notice',[62,23],7,'note','DO NOT LEAVE GLUE OPEN.\n\nOne stick can fix three torn pieces of an ID. It cannot fix your GPA.\nOpen inventory with TAB, select all four materials, then COMBINE.')
obj('hide_west','Hide under desk',[18,10],8,'hide')
obj('hide_hub','Hide in locker',[29,11],10,'hide')
obj('hide_east','Hide in service locker',[69,29],10,'hide')
decor=[]
def prop(x,y,tile):decor.append([x,y,tile])
for x in (5,9,13,17,21):
    prop(x,3,9);prop(x,8,9)
for x in (6,10,14):prop(x,11,8)
for x in (30,34,38,42):
    for y in (5,9):prop(x,y,8);prop(x,y+1,13)
for x in (5,8,10):
    for y in (26,29):prop(x,y,8)
for x in (56,59,62):
    for y in (26,29):prop(x,y,8)
for x in (56,60,64,68,72):prop(x,11,31)
for x in (58,63,68):prop(x,8,30)
for x in (16,19,22):prop(x,22,15)
for x in (68,71,74):prop(x,22,15)
for x,y in [(29,22),(46,22),(29,30),(46,30),(21,12),(73,12)]:prop(x,y,29)
for x in range(5,76,8):prop(x,15,14)
for x,y in [(32,27),(44,27)]:prop(x,y,13)
write('data/map_layout.json',dict(tile_size=32,size=[78,36],corridor=[2,15,74,4],rooms=rooms,doors=doors,objects=objects,decor=decor,lamps=[[x*32+16,544] for x in range(6,76,10)]+[[400,280],[1200,280],[2080,280],[1200,840]],start=[1200,320],checkpoints=[[800,544],[1280,544],[1696,544]],shade_spawn=[544,544],patrol=[[224,544],[736,544],[1216,544],[1696,544],[2304,544],[1696,544],[1216,544],[736,544]]))
items={
'west_key':{'name':'Librarian drawer key','description':'Found in a restroom stall. Academically questionable. Opens the library desk.','tile':19},
'east_key':{'name':'Canteen key','description':'The fridge key. Definitely not a snack.','tile':20},
'piece_1':{'name':'ID card / piece 1 of 3','description':'The photo corner. I look exactly as tired as I feel.','tile':16},
'piece_2':{'name':'ID card / piece 2 of 3','description':'My student number. Finally, proof that I exist.','tile':17},
'piece_3':{'name':'ID card / piece 3 of 3','description':'The chip. Slightly cold, but hopefully functional.','tile':18},
'glue':{'name':'Glue stick','description':'Select this and all three ID pieces, then press Combine.','tile':21},
'restored_id':{'name':'Restored ID card','description':'Three pieces, one identity, zero remaining excuses. Return to the atrium exit.','tile':22}}
write('data/items.json',items)
write('data/dialogue.json',{
'intro':'02:14 AM.\n\nI fell asleep over the last slide. My phone is dead. The doors are locked. My student ID is in three pieces.\n\nPA: "The university is now closed. Any remaining students are reminded that deadlines are not a valid reason to remain alive in the building."\n\nFind the pieces. Find glue. Get out before the night shift finds you.',
'flashlight':'A flashlight. Finally, a bright idea that does not need a citation.',
'shade':'That is not campus security. Campus security has shoes.',
'west_key':'A tiny key in a restroom. I am choosing not to think about it.',
'east_key':'Canteen key acquired. Please let the fridge contain my ID, not another assignment.',
'piece_1':'One third of a student. Still enrolled, apparently.',
'piece_2':'Two pieces. The locker has better password security than my email.',
'piece_3':'All three pieces. My identity has survived refrigeration.',
'glue':'Glue. The final group-project member who actually shows up.',
'locked_library_drawer':'Locked. The library note says the drawer key was lost in the west restroom.',
'locked_fridge':'Locked. The canteen key should be on the teaching desk in Classroom 103.',
'wrong_code':'Wrong code. Four digits, west classroom whiteboard. Panic is not a fifth digit.',
'correct_code':'Click. Weeks, room, group, extensions. 2-4-1-0.',
'combine':'Identity restored. The glue is holding. So am I. Main exit, atrium.',
'exit_locked':'The scanner wants one whole student ID. Find three pieces and glue; combine them in inventory.',
'caught':'The night shift found me. Back at the block entrance. At least it did not take my stuff.',
'escape':'The turnstile beeps. Dawn. I have never been so happy to see an 8 AM lecture.',
'empty':'Nothing else here. For once, that is good news.',
'hide':'Hidden. Stay still. Press E to leave when the corridor is quiet.',
'unhide':'Back out. Deep breath. Quiet footsteps.'})
for name in ('block1','block2','block3','corridors'):
    p=root/f'scenes/world/{name}.tscn';p.parent.mkdir(parents=True,exist_ok=True)
    p.write_text(f'[gd_scene format=3]\n\n[node name="{name}" type="Node2D"]\n',encoding='utf-8')
print('Generated map and original English narrative')
