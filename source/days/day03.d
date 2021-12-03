module days.day03;

import std.algorithm : count, each, filter, fold, map;
import std.array : array;
import std.bitmanip : BitArray;
import std.conv : to;
import std.functional : pipe;
import std.range : iota, transposed;
import std.stdio : writefln, writeln;
import std.string : splitLines, strip;
import containers : HashSet;
import days.day : Day;
import timer : Timer;

bool[] parseBits(S)(S s)
{
    return s.map!(c => c == '0' ? false : true).array;
}

class Day03 : Day!(BitArray[], ulong, "data/03.txt")
{
    BitArray[] parseData(string data, Timer* timer)
    {
        auto lines = data.splitLines.map!strip.array;
        auto fields = lines.transposed.map!(pipe!(parseBits, BitArray)).array;
        assert(fields.length);
        return fields;
    }

    ulong problemA(BitArray[] data, Timer* timer)
    {
        struct State
        {
            ulong gamma;
            ulong epsilon;
        }

        State initialState = State(0, 0);

        State folder(State acc, BitArray elt)
        {
            ulong onesSet = elt.count;
            elt.flip;
            ulong zeroesSet = elt.count;
            elt.flip;
            ulong mostCommon = onesSet > zeroesSet ? 1 : 0;
            return State((acc.gamma << 1) + mostCommon, (acc.epsilon << 1) + !mostCommon);
        }

        State finalState = data.fold!folder(initialState);
        return finalState.gamma * finalState.epsilon;
    }

    ulong problemB(BitArray[] data, Timer* timer)
    {
        HashSet!ulong oxyRows;
        data.length.iota.each!(i => cast(void)oxyRows.put(i));
        HashSet!ulong co2Rows;
        data.length.iota.each!(i => cast(void)co2Rows.put(i));
        oxyRows[].writeln;
        co2Rows[].writeln;
        for (size_t i = 0; i < data.length; i++)
        {
            ulong onesSet = data[i].bitsSet.filter!(i => oxyRows.contains(i)).count;
            data[i].flip;
            ulong zeroesSet = data[i].bitsSet.filter!(i => co2Rows.contains(i)).count;
            data[i].flip;

            if (oxyRows.length > 1)
            {
                ulong oxyBit = onesSet >= zeroesSet;
                writefln("i = %s, oxyBit = %s", i, oxyBit);
                data[i].bitsSet.filter!(b => data[i][b] == oxyBit).each!(i => cast(void)oxyRows.remove(i));
            }
            if (co2Rows.length > 1)
            {
                ulong co2Bit = zeroesSet >= onesSet;
                data[i].bitsSet.filter!(b => data[i][b] == co2Bit).each!(i => cast(void)co2Rows.remove(i));
            }
        }
        oxyRows[].writeln;
        co2Rows[].writeln;
        return 0;
    }

    unittest
    {
        Day03 day = new Day03;
        string input = "00100\n11110\n10110\n10111\n10101\n01111\n00111\n11100\n10000\n11001\n00010\n01010";
        auto data = day.parseData(input, null);
        // The timer isn't used for these solutions so can be ignored
        assert(day.problemA(data, null) == 198);
        assert(day.problemB(data, null) == 230);
    }
}
