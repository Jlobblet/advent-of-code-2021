module days.day02;
import std.algorithm : fold, map, sum;
import std.array : array;
import std.conv : to;
import std.functional : compose;
import std.string : split, splitLines, strip;
import std.uni : isWhite;
import days.day : Day;
import timer : Timer;

enum Direction
{
    Forward,
    Down,
    Up,
}

struct Command
{
    Direction direction;
    long magnitude;
}

Command parseCommand(string input)
{
    auto atoms = input.strip.split!isWhite;
    long magnitude = atoms[1].to!long;
    Direction direction;
    final switch (atoms[0].strip)
    {
        case "forward":
            direction = Direction.Forward;
            break;
        case "down":
            direction = Direction.Down;
            break;
        case "up":
            direction = Direction.Up;
            break;
    }
    return Command(direction, magnitude);
}

struct Vector
{
    long horizontal;
    long vertical;

    Vector opBinary(string op)(const Vector rhs) const
    {
        static if (op == "+") return Vector(horizontal + rhs.horizontal, vertical + rhs.vertical);
        else static if (op == "-") return Vector(horizontal - rhs.horizontal, vertical - rhs.vertical);
        else assert(false, "Operator " ~ op ~ "not implemented for Vector.");
    }
}

Vector toVector(Command input)
{
    switch(input.direction) {
        case Direction.Forward:
            return Vector(input.magnitude, 0);
        case Direction.Down:
            return Vector(0, input.magnitude);
        case Direction.Up:
            return Vector(0, -input.magnitude);
        default:
            throw new Exception("Unknown direction");
    }
}

class Day02 : Day!(Command[], long, "data/02.txt")
{
    Command[] parseData(string data, Timer* timer)
    {
        return data.splitLines.map!(compose!(parseCommand, strip)).array;
    }

    long problemA(Command[] data, Timer* timer)
    {
        Vector finalPosition = data.map!(toVector).sum;
        return finalPosition.horizontal * finalPosition.vertical;
    }

    long problemB(Command[] data, Timer* timer)
    {
        struct State
        {
            Vector position;
            long aim;
        }

        State folder(State acc, Command elt)
        {
            final switch (elt.direction)
            {
                case Direction.Forward:
                    return State(
                        Vector(
                            acc.position.horizontal + elt.magnitude,
                            acc.position.vertical + acc.aim * elt.magnitude),
                        acc.aim
                    );
                case Direction.Down:
                    return State(acc.position, acc.aim + elt.magnitude);
                case Direction.Up:
                    return State(acc.position, acc.aim - elt.magnitude);
            }
        }

        State initialState = State(Vector(0, 0), 0);
        State finalState = data.fold!folder(initialState);

        return finalState.position.horizontal * finalState.position.vertical;
    }

    unittest
    {
        Day02 day = new Day02;
        string input = "forward 5\ndown 5\nforward 8\nup 3\ndown 8\nforward 2";
        Command[] data = day.parseData(input, null);
        // The timer isn't used for these solutions so can be ignored
        assert(day.problemA(data, null) == 150);
        assert(day.problemB(data, null) == 900);
    }
}
