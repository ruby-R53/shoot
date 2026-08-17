# Shoot!

This is an attempt at making a vertical shooting game in ncurses. It's in a
very crude state, and currently some more polishing is needed.

## Story

You're a mad scientist who decided to wander around the cosmos using blueprints
you stole for highly advanced spaceships, that have weapons built into them.
This ends up being very useful, because you may have some··· encounters during
your adventure!

I guess that's it for now.

## How it works

- ~~The game works in a turn-based fashion. This means, each move you make means
that the opponent must make one as well. If you don't do anything, both of you
will remain still, until *you* decide to make a move.~~
This branch seeks to make the game work like any other: the enemy moves
independently from you. There's still a long way to make this more pleasant to
play, however. But it's something. With that said:
    - The enemy makes a move every .1 seconds. Because of the terminal nature of
      ncurses however, if you have a keyboard repeat rate that's higher than this,
      the updates will happen more often to match that.

- The center of your sprite, `-=^=-`, is where the bullets get spat out from.
To shoot them, press the `Z` key. No need to worry about running out of them!

- ~~You may also give your opponent the turn by pressing `X`. This skips your
turn and makes your opponent do something instead. Useful in case you can't
reach your opponent, which might happen quite often because of the
poorly-implemented RNG-based moves :)~~
This is also unecessary in this branch. Although, like the subitem mentioned,
you may be able to "fast-forward" the game depending on your keyboard settings.
In which case, it doesn't matter which key you press.

- You start with 4 HP. Your opponent's HP is the result of the current level
plus 2, just for an extra challenge.

- By default, you get 12 levels to play. At every 2 levels, or, more precisely,
at every odd-numbered level, 1 is added to your HP.
    - You may run `shoot #` to have `#` levels instead!

- Press `Q` to quit at any time.

## Building

Just run `make` inside the game's directory. You can then run it with `./shoot`.

If you want to however, you may also change `LVL_MAX` in `shoot.h` to change
the default amount of levels. You can launch it with a number argument to set
that as well, but you do have a choice for a more fixed number.

## Contributing

Pull requests, issues, anything is welcome. Or rather, _needed_, because I'm
not very experienced at this···
