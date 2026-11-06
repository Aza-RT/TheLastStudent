"""Original deterministic 32px pixel-art atlas; requires Pillow (pip install Pillow)."""
from pathlib import Path
from PIL import Image, ImageDraw
import random
ROOT=Path(__file__).resolve().parents[1]
OUT=ROOT/'game/assets/art'
OUT.mkdir(parents=True,exist_ok=True)
rng=random.Random(2401)
atlas=Image.new('RGBA',(256,256),(0,0,0,0))
def tile(index):
    image=Image.new('RGBA',(32,32),(0,0,0,0))
    return image,ImageDraw.Draw(image)
def save(i,im): atlas.paste(im,((i%8)*32,(i//8)*32))
for i in range(64):
    im,d=tile(i)
    if i<3:
        d.rectangle((0,0,31,31),fill=['#414b51','#3a454d','#475157'][i])
        d.line((0,0,31,0),fill='#5a6468');d.line((0,0,0,31),fill='#596268')
        d.line((31,0,31,31),fill='#29343c');d.line((0,31,31,31),fill='#27323a')
        for _ in range(14):
            x,y=rng.randrange(2,30),rng.randrange(2,30);d.point((x,y),fill='#505b60')
        if i==2:d.line((13,0,16,7,14,13),fill='#2d383f')
    elif i==3:
        d.rectangle((0,0,31,31),fill='#222e3c');d.rectangle((1,1,30,25),fill='#394c5b')
        d.line((1,1,30,1),fill='#73818a',width=2);d.line((0,29,31,29),fill='#121d29',width=3)
        d.line((16,3,16,24),fill='#30414f');d.line((2,13,29,13),fill='#30414f')
    elif i in (4,5,6,7):
        d.rectangle((2,3,29,29),fill='#202e38');d.rectangle((3,2,28,26),fill='#586b73')
        if i==4:
            d.rectangle((5,5,26,12),fill='#86b5a3');d.rectangle((6,16,25,24),fill='#2f484a')
        if i==5:
            d.rectangle((6,6,26,22),fill='#22393e');d.line((8,9,24,9),fill='#b4cbb5',width=2)
        if i==6:
            d.rectangle((5,5,27,24),fill='#738d80');d.rectangle((8,7,24,15),fill='#253b44');d.ellipse((22,19,25,22),fill='#d4bc75')
        if i==7:
            d.rectangle((6,5,25,21),fill='#9eaea7');d.line((8,10,23,10),fill='#5c7772');d.line((8,15,20,15),fill='#5c7772')
    elif i in (8,9): # desk/bookshelf
        d.rectangle((2,6,30,29),fill='#18252c');d.rectangle((1,2,29,25),fill='#685e54');d.line((2,3,28,3),fill='#a39a7b')
        if i==8:
            d.rectangle((5,5,15,13),fill='#b9beb0');d.rectangle((18,6,25,15),fill='#304751');d.ellipse((21,18,26,23),fill='#9a9b83')
        else:
            for y in (6,16):
                for x in range(4,27,4):d.rectangle((x,y,x+2,y+7),fill=rng.choice(['#92796a','#698b87','#b4a477','#6c7b96']))
            d.line((3,14,27,14),fill='#242e32')
    elif i==10: # lockers
        d.rectangle((2,2,29,29),fill='#263743');d.rectangle((3,2,28,26),fill='#64797b')
        for x in (4,17):
            d.rectangle((x,4,x+10,24),outline='#3a535d');d.line((x+2,7,x+8,7),fill='#293e49');d.rectangle((x+7,14,x+8,17),fill='#cfba80')
    elif i==11: # fridge
        d.rectangle((3,1,29,29),fill='#233744');d.rectangle((3,1,27,26),fill='#9caaa4');d.line((4,12,26,12),fill='#536d73');d.line((23,5,23,10),fill='#324e58',width=2);d.line((23,16,23,23),fill='#324e58',width=2)
    elif i==12: # turnstile
        d.rectangle((2,4,10,28),fill='#859a96');d.rectangle((23,4,30,28),fill='#607d7c');d.rectangle((3,5,9,9),fill='#d76a5d');d.line((10,15,22,21),fill='#c1d0c0',width=3);d.line((15,18,20,9),fill='#c1d0c0',width=3)
    elif i==13: # bench
        d.rectangle((1,4,30,27),fill='#263440');d.rectangle((2,3,29,22),fill='#526d6c');d.line((3,5,28,5),fill='#8ca393');d.line((16,6,16,21),fill='#344c50')
    elif i==14: # lamp
        d.rectangle((5,12,28,20),fill='#344b51');d.rectangle((6,10,27,16),fill='#d3d7ad');d.line((7,10,26,10),fill='#faf3c7')
    elif i==15: # toilet
        d.rectangle((9,2,23,10),fill='#a9b7b0');d.ellipse((7,7,25,28),fill='#8ca3a0');d.ellipse((10,10,22,24),fill='#304954');d.line((10,3,21,3),fill='#d1d6bc')
    elif 16<=i<24: # inventory icons
        if i<19:
            d.polygon([(6,5),(25,5),(24,23),(16,27),(13,23),(6,24)],fill=['#c5d7b7','#97c1bd','#b8c0d0'][i-16]);d.line((8,8,22,8),fill='#497982',width=2);d.rectangle((9,12,15,18),fill='#4c646c');d.line((17,14,22,14),fill='#667f7b')
        elif i in (19,20):
            d.ellipse((4,7,15,18),outline='#d4bd79',width=3);d.line((13,16,25,27),fill='#d4bd79',width=3);d.line((20,20,24,16),fill='#d4bd79',width=2)
        elif i==21:
            d.rectangle((11,7,22,27),fill='#c3d0b0');d.rectangle((10,4,23,11),fill='#b88a61');d.rectangle((12,15,21,21),fill='#5e9389')
        elif i==22:
            d.rounded_rectangle((3,5,29,26),radius=3,fill='#c3d8bd');d.rectangle((6,10,14,20),fill='#4e7377');d.line((17,11,26,11),fill='#477570',width=2);d.line((17,17,24,17),fill='#6c9790');d.line((5,23,27,23),fill='#7aa59a')
        else:
            d.rectangle((8,3,25,28),fill='#c0c6a7');d.line((11,8,22,8),fill='#4a6c68');d.line((11,13,22,13),fill='#4a6c68');d.line((11,18,20,18),fill='#4a6c68')
    elif 24<=i<28: # Shade with ragged robe / no human face
        frame=i-24;d.ellipse((7,3,25,19),fill='#617e88');d.polygon([(8,13),(24,13),(28,28-frame),(22,25),(17,30),(12,26),(5,29-frame)],fill='#344d61');d.ellipse((10,7,22,19),fill='#182637');d.point((12,12),fill='#bfd6cd');d.point((20,12),fill='#bfd6cd')
    elif i==28: # coffee
        d.rectangle((10,8,23,24),fill='#acb5a0');d.arc((18,9,28,21),-90,90,fill='#acb5a0',width=2);d.line((12,5,14,1),fill='#8ca49c')
    elif i==29: # plant
        d.rectangle((11,18,22,29),fill='#7a7160');d.line((16,5,16,21),fill='#647d62',width=2)
        for x,y in [(7,7),(16,2),(17,10),(6,14)]:d.ellipse((x,y,x+9,y+7),fill='#4b7168')
    elif i==30:
        d.rectangle((1,6,30,27),fill='#38545b');d.rectangle((3,3,28,23),fill='#91a79d');d.line((3,5,28,5),fill='#b7c7b0');d.rectangle((9,8,23,19),fill='#2e4953')
    elif i==31:
        d.rectangle((1,5,30,28),fill='#485f60');d.rectangle((2,3,29,23),fill='#9b9b80');d.line((4,7,27,7),fill='#c5baa0');d.line((4,18,27,18),fill='#716f61')
    elif 32<=i<48:
        direction=(i-32)//4;frame=(i-32)%4;swing=[0,2,0,-2][frame]
        d.ellipse((9,25,24,30),fill='#1c2936');d.rectangle((11,21,15,28+swing//2),fill='#273e52');d.rectangle((19,21,23,28-swing//2),fill='#273e52')
        d.rectangle((10,11,23,23),fill='#6d9993');d.line((11,12,22,12),fill='#a0bcb0');d.rectangle((8,13,11,20+swing),fill='#547e7b');d.rectangle((23,13,26,20-swing),fill='#547e7b')
        d.rectangle((11,3,22,12),fill='#bbaa8d');d.rectangle((10,2,23,6),fill='#273342');d.rectangle((11,2,22,3),fill='#4a4e4a')
        if direction==0:d.rectangle((9,11,17,21),fill='#3c535c')
        elif direction==1:d.point((20,8),fill='#223440');d.point((14,8),fill='#223440')
        elif direction==2:d.point((22,8),fill='#223440')
        else:d.point((11,8),fill='#223440')
    elif i==48: # unlocked turnstile, arms folded clear of the lane
        d.rectangle((2,4,10,28),fill='#859a96');d.rectangle((23,4,30,28),fill='#607d7c');d.rectangle((3,5,9,9),fill='#99d3a4');d.line((10,12,10,26),fill='#c1d0c0',width=3);d.line((22,12,22,26),fill='#c1d0c0',width=3)
    save(i,im)
atlas.save(OUT/'atlas.png')
# Original hand-specified 5x7 glyphs; no font file or external font is redistributed.
glyphs={
'A':(14,17,17,31,17,17,17),'B':(30,17,17,30,17,17,30),'C':(14,17,16,16,16,17,14),
'D':(30,17,17,17,17,17,30),'E':(31,16,16,30,16,16,31),'F':(31,16,16,30,16,16,16),
'G':(14,17,16,23,17,17,14),'H':(17,17,17,31,17,17,17),'I':(14,4,4,4,4,4,14),
'J':(7,2,2,2,18,18,12),'K':(17,18,20,24,20,18,17),'L':(16,16,16,16,16,16,31),
'M':(17,27,21,21,17,17,17),'N':(17,25,25,21,19,19,17),'O':(14,17,17,17,17,17,14),
'P':(30,17,17,30,16,16,16),'Q':(14,17,17,17,21,18,13),'R':(30,17,17,30,20,18,17),
'S':(15,16,16,14,1,1,30),'T':(31,4,4,4,4,4,4),'U':(17,17,17,17,17,17,14),
'V':(17,17,17,17,17,10,4),'W':(17,17,17,21,21,27,17),'X':(17,17,10,4,10,17,17),
'Y':(17,17,10,4,4,4,4),'Z':(31,1,2,4,8,16,31),
'0':(14,17,19,21,25,17,14),'1':(4,12,4,4,4,4,14),'2':(14,17,1,2,4,8,31),
'3':(30,1,1,14,1,1,30),'4':(2,6,10,18,31,2,2),'5':(31,16,16,30,1,1,30),
'6':(14,16,16,30,17,17,14),'7':(31,1,2,4,8,8,8),'8':(14,17,17,14,17,17,14),
'9':(14,17,17,15,1,1,14),'.':(0,0,0,0,0,12,12),'-':(0,0,0,31,0,0,0),
'/':(1,2,2,4,8,8,16),':':(0,12,12,0,12,12,0),'*':(0,21,14,31,14,21,0),
'=':(0,31,0,31,0,0,0),'+':(0,4,4,31,4,4,0),'[':(14,8,8,8,8,8,14),']':(14,2,2,2,2,2,14),
'(':(2,4,8,8,8,4,2),')':(8,4,2,2,2,4,8),'>':(16,8,4,2,4,8,16),'_':(0,0,0,0,0,0,31),
'?':(14,17,1,2,4,0,4),' ':(0,0,0,0,0,0,0)}
font=Image.new('RGBA',(128,64),(0,0,0,0));fd=ImageDraw.Draw(font)
for c in range(32,128):
    x,y=((c-32)%16)*8,((c-32)//16)*10
    for row,bits in enumerate(glyphs.get(chr(c).upper(),glyphs['?'])):
        for col in range(5):
            if bits&(16>>col):fd.point((x+col+1,y+row+1),fill='#d9e8d2')
font.save(OUT/'debug_font.png')
print('Generated atlas.png (64 cells), debug_font.png')
