module days.day01;

import std.algorithm : fold, map, sum;
import std.array : array;
import std.conv : to;
import std.functional : compose;
import std.range : drop, dropOne;
import std.string : splitLines, strip;
import std.typecons : tuple;
import days.day : Day;
import timer : Timer;

class Day01 : Day!(ulong[], ulong, "data/01.txt")
{
    ulong[] parseData(string data, Timer* timer)
    {
        return data.splitLines.map!(compose!(to!ulong, strip)).array;
    }

    ulong problemA(ulong[] data, Timer* timer)
    {
        // Struct to store fold accumulator.
        struct State
        {
            ulong Last;
            ulong Count;
        }

        // Start with the first element as the max value.
        State initialState = State(data[0], 0);

        State folder(State acc, ulong elt)
        {
            bool larger = elt > acc.Last;
            // If this element is larger than the last element update the state.
            return State(elt, acc.Count + larger);
        }

        return data.dropOne.fold!folder(initialState).Count;
    }

    ulong problemB(ulong[] data, Timer* timer)
    {
        struct State
        {
            ulong[3] LastThree;
            ulong CurrentSum;
            ulong Count;
        }

        State initialState = State(data[0..3], data[0..3].sum, 0);

        State folder(State acc, ulong elt)
        {
            ulong nextSum = acc.LastThree[1] + acc.LastThree[2] + elt;
            ulong[3] nextThree = [acc.LastThree[1], acc.LastThree[2], elt];
            bool larger = nextSum > acc.CurrentSum;
            return State(nextThree, nextSum, acc.Count + larger);
        }

        return data.drop(3).fold!folder(initialState).Count;
    }
}
