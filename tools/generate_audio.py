"""Original deterministic mono PCM synthesizer. No external dependencies."""
import math,random,wave,struct
from pathlib import Path
out=Path(__file__).resolve().parents[1]/'game/assets/audio';out.mkdir(parents=True,exist_ok=True)
rate=22050;rng=random.Random(2401)
def create(name,seconds,fn):
    with wave.open(str(out/(name+'.wav')),'wb') as w:
        w.setparams((1,2,rate,0,'NONE','not compressed'))
        data=[]
        for i in range(int(seconds*rate)):
            t=i/rate;data.append(struct.pack('<h',int(max(-1,min(1,fn(t,seconds)))*26000)))
        w.writeframes(b''.join(data))
create('hum',4,lambda t,s:(.16*math.sin(t*math.tau*55)+.08*math.sin(t*math.tau*110)+rng.uniform(-.02,.02))*(.9+.1*math.sin(t*math.tau/4)))
create('step',.19,lambda t,s:rng.uniform(-1,1)*.45*math.exp(-t*32)+.15*math.sin(t*math.tau*90)*math.exp(-t*28))
create('heartbeat',.8,lambda t,s:sum(.48*math.sin(t*math.tau*55)*math.exp(-max(0,t-d)*25) if t>d else 0 for d in (0,.19)))
create('click',.08,lambda t,s:rng.uniform(-1,1)*math.exp(-t*80)*.5)
create('pickup',.5,lambda t,s:.24*math.sin(t*math.tau*(660 if t<.17 else 990))*math.sin(math.pi*t/s)**2)
create('lock',.3,lambda t,s:(.22*math.sin(t*math.tau*130)+rng.uniform(-.1,.1))*math.exp(-t*14))
create('sting',1.5,lambda t,s:(.21*math.sin(t*math.tau*(170+170*t))+.15*math.sin(t*math.tau*(181+173*t))+rng.uniform(-.08,.08))*math.sin(math.pi*t/s))
create('escape',2,lambda t,s:sum(.12*math.sin(t*math.tau*f) for f in (261.63,329.63,392))*math.sin(math.pi*t/s)**2)
print('Generated 8 original PCM sounds')
