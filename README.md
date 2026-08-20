# Shoot!

This is an attempt at making a vertical shooter game in ncurses. It's currently
in a very crude state, and so some more polishing is needed.

## Story

You're a mad scientist who decided to wander around the cosmos using blueprints
you stole for highly advanced spaceships, that have weapons built into them.
This ends up being very useful, because you may have some··· encounters during
your adventure!

I guess that's it for now.

## How it works

- Use the arrow keys to move. Additionally, you may use Vim keys (`hjkl`) to
move as well.

- By default, you have 12 levels to play. The game gets progressively harder on
each level by adding 1 more HP to the enemy, which you also do only get one for
each level.
  - You may temporarily change the amount of levels by launching `shoot` with a
  number argument. So `./shoot 6` makes the game only have 6 levels instead.
  - You may then permanently change the amount of levels in the game by
  changing the `LVL_MAX` macro in `shoot.h`. It is that easy.
  - The enemy's moves are picked entirely at random. It doesn't have proper
  reasoning yet, although it may feel like it at times :)

- Both you and the enemy have an infinite amount of bullets. That is, until
either of you die.
  - Press `z` to shoot. Each bullet hit only takes 1 HP from the target. There
  may be new projectiles in the future that take more HP out or buff items for
  each player, mostly for restoring HP.

- The game runs at around 20 FPS (an update rate of 50 milliseconds), although
I may change this later on as I tweak the game further.
  - It may feel quite slow/sluggish to play on, but this is so that the enemy
  doesn't move like crazy. I may try a better approach to this in the future.

- You start with 4 HP. The enemy's HP is calculated by adding 5 to the current
level. So for level 7, that means the enemy has 12 HP.
  - However, at every 2 levels, you do get 1 extra HP to keep up with the enemy
  getting more and more resistant to your attacks.

- Press `p` to pause the game. Nothing but a small line of text will appear on
your screen, and you may press any key to continue.

- Press `q` to quit at any time during the game. Beware that no confirmation
screen will appear, and your progress isn't saved!

## Building

Just run `make` inside the game's directory. You can then run it with `./shoot`.

## Contributing

Pull requests, issues, anything is welcome.
