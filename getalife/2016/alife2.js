
// Matter.js module aliases
var Engine = Matter.Engine,
    World = Matter.World,
    Bodies = Matter.Bodies,
    Events = Matter.Events,
    Common = Matter.Common;

// create a Matter.js engine
var engine = Engine.create({
    render: {
        element: document.body,
        controller: RenderAlife,
        options: {
            width: window.innerWidth-4,
            height: window.innerHeight-4,
            enableSleeping: true,
        }
    }
});

engine.timing.isFixed = true;
engine.world.bounds.max.x = window.innerWidth;
engine.world.bounds.max.y = window.innerHeight;
engine.world.gravity.y = 0;
engine.world.gravity.x = 0;

var walls = [
    Bodies.rectangle(window.innerWidth/2, window.innerHeight, window.innerWidth, 1, { isStatic: true }),
    Bodies.rectangle(window.innerWidth/2, -1, window.innerWidth, 1, { isStatic: true }),
    Bodies.rectangle(-1, window.innerHeight/2, 1, window.innerHeight, { isStatic: true }),
    Bodies.rectangle(window.innerWidth, window.innerHeight/2, 1, window.innerHeight, { isStatic: true }),
];

World.add(engine.world, walls);

S_ALIVE = 1;
S_DYING = 2;
S_DEAD = 3;

Common._seed = Math.random() * (0xffffffffff);

function randomFloat(min, max) {
//    Common._seed = Math.random() * (0xffffffff - 0);
    return Common.random(min,max);
//    return Math.random() * (max - min) + min;
}
function randomInt(min, max) {
    return Math.floor(Math.random() * (max - min)) + min;
}
CreatureUpdate = function(body) {
    if (body.state == S_ALIVE) {
        if ((body.tick % body.tickCycle) == 0) {
            
            if (randomInt(0, body.erratic) == 0) {
                body.torque += randomFloat(-0.1, 0.1) * body.mass;
            }
            // Once in a while, make a major turn
            if (randomInt(0, 50) == 0) {
                body.torque += randomFloat(-0.5, 0.5) * body.mass;
            }
            // Move in direction we're facing
            Matter.Body.applyForce(body, body.position,
                                   { x: (Math.cos(body.angle) * body.power) * body.mass,
                                     y: (Math.sin(body.angle) * body.power) * body.mass });
            // Grow up
            if (body.mass < 1.0) {
                this.lastmass = this.mass;
                Matter.Body.scale(body, 1.008, 1.008);
                this.power *= 1.008;
            }
            if (body.tick > body.maxTick) {
                body.state = S_DYING;
            }
        };

    } else if (body.state == S_DYING) {
        if (body.tick - body.maxTick > 150) {
            body.state = S_DEAD;
        }
    }
    body.tick += 1;
}

CreatureRenderer = function(container) {
    if (this.state == S_DYING && !this.dyingfilter) {
        this.dyingfilter = new PIXI.filters.GrayFilter();
        this.dyingfilter.gray = 1;
        this.g.filters = [this.dyingfilter];
    }

    // Blurry background
    if (this.mass > 0.7) {
        var bg = this.bg;

        if (this.mass != this.lastmass) {
            bg.clear();
            bg.tint = this.tint;
            // In fill
            bg.beginFill(0x888888, 1);
            bg.drawEllipse(-5, 0, (-36/-0.3) * this.mass - 84, (-30/-0.3) * this.mass - 70);
            bg.endFill();
        }

        if (this.state == S_DYING) {
            bg.alpha = 1 - ((this.tick-this.maxTick) * 0.01);
        } else {
            bg.filters[0].blur = ((-50/-0.3) * this.mass) - 116.666;
        }

        bg.rotation = this.angle;
        bg.x = this.position.x;
        bg.y = this.position.y;

        container.addChild(bg);
    }
    var g = this.g;
    g.clear();
    g.tint = this.tint;
    if (this.state == S_DYING) {
        g.alpha = 1 - ((this.tick-this.maxTick) * 0.02);
        g.tint = 0xffffff;
    }

    g.rotation = this.angle;
    g.x = this.position.x;
    g.y = this.position.y;
    if (this.mass >= 0.4) {
        // Inner body
        g.lineStyle(10 * this.mass, 0xaaaaaa, ((-1.0/-0.6) * this.mass) - 0.66);
        g.drawEllipse(-6, 0, (24+this.speed) * this.mass, 18 * this.mass);
        // mitochondria stuff
        g.lineStyle(2*this.mass, 0xffffff, (0.5 + (Math.sin(this.tick*0.1) * 0.3))*this.mass);
        for (i=0 ; i<3; i++) {
            var x = 5*Math.cos(this.tick*0.2+((Math.PI*2)/3*i) % (Math.PI*2))-8;
            var y = 5*Math.sin(this.tick*0.2+((Math.PI*2)/3*i) % (Math.PI*2));
            g.drawCircle(x*this.mass,y*this.mass,3*this.mass,3*this.mass);
        }
    }
    if (this.mass > 0.6) {
        // Main body
        g.lineStyle(8 * this.mass, 0xffffff, ((-1.0/-0.4) * this.mass) - 1.5);
        g.drawEllipse(0, 0, 28 * this.mass, 23 * this.mass);
    }
    // eye
    g.lineStyle(6 * this.mass, 0xffffff, Common.clamp(1*this.mass, 0.50, 1.0));
    g.drawEllipse((Math.min(22, 14+this.speed)) * this.mass, 0, 8 * this.mass, (14-Math.min(8.5,this.speed)) * this.mass);

    // wireframe
/*    radius = Math.sqrt(this.area/3.14259)
    g.lineStyle(1, 0xffffff, 1);
    g.drawCircle(0,0,radius); */
    container.addChild(g);
}

function Creature() {
    var body = Bodies.circle(randomFloat() * window.innerWidth, randomFloat() * window.innerHeight, 30, {density: 0.0003});
    Matter.Body.setAngle(body, randomFloat(1, 360));
    body.frictionAir = 0.1;
    body.restitution = 0.8;
    body.erratic = randomInt(1,20);
    body.power = randomFloat(0.005, 0.01) * 0.30;
    Matter.Body.scale(body, 0.30, 0.30);

    body.tickCycle = randomInt(5,50);
    body.tick = randomInt(0, body.tickCycle);
    body.maxTick = randomInt(body.tickCycle*300, body.tickCycle*500);
    body.state = S_ALIVE;

    body.tint = randomInt(0,0xffffff);
    body.g = new PIXI.Graphics();
    body.bg = new PIXI.Graphics();
    body.bg.filters = [new PIXI.filters.BlurFilter()];
    body.bg.filters[0].blur = 50;
    body.Update = CreatureUpdate;
    body.CustomRenderer = CreatureRenderer;
    return body;
}

var creatures = [];
startCreatures = Math.floor(Math.min(10, window.innerHeight * window.innerWidth / 14000));
for (i = 0; i < startCreatures ; i++) {
    var c = new Creature();
    creatures.push(c);
    World.add(engine.world, c);
}

Events.on(engine, 'beforeTick', function() {
    creatures.forEach(function(c, i) {
        if (c == undefined) {
            return;
        }
        if (c.state == S_ALIVE || c.state == S_DYING) {
            c.Update(c);
        } else {
            creatures[i] = undefined;
            Matter.Composite.removeBody(engine.world, c);
        }
    });

});



Engine.run(engine);

