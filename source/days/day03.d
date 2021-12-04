module days.day03;

import std.algorithm : canFind, count, each, filter, fold, map;
import std.array : array;
import std.bitmanip : BitArray;
import std.conv : to;
import std.functional : pipe;
import std.range : iota, takeOne, transposed;
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
            ulong mostCommon = onesSet > zeroesSet;
            return State((acc.gamma << 1) + mostCommon, (acc.epsilon << 1) + !mostCommon);
        }

        State finalState = data.fold!folder(initialState);
        return finalState.gamma * finalState.epsilon;
    }

    ulong problemB(BitArray[] data, Timer* timer)
    {
        bool[] initialArr = new bool[data[0].length];
        initialArr[0 .. data[0].length] = true;
        // Filters for which rows are still valid
        BitArray oxyRows = BitArray(initialArr.array);
        BitArray co2Rows = BitArray(initialArr.array);
        for (size_t i = 0; i < data.length; i++)
        {
            // Count 0s and 1s for each filter
            size_t oxyOnesSet = data[i].bitsSet.filter!(j => oxyRows[j]).count;
            size_t co2OnesSet = data[i].bitsSet.filter!(j => co2Rows[j]).count;
            data[i].flip;
            size_t oxyZeroesSet = data[i].bitsSet.filter!(j => oxyRows[j]).count;
            size_t co2ZeroesSet = data[i].bitsSet.filter!(j => co2Rows[j]).count;
            data[i].flip;

            if (oxyRows.count > 1)
            {
                bool oxyBit = oxyOnesSet >= oxyZeroesSet;

                // If the bit for this iteration is 1, flip
                if (oxyBit) {
                    data[i].flip;
                }
                // Now remove all of the 1s from this column from the filter
                // Because of the flip earlier, the bit to keep is now always 0
                // and the bit to discard is always 1
                data[i].bitsSet.each!(i => oxyRows[i] = false);
                // Flip back
                if (oxyBit) {
                    data[i].flip;
                }
            }

            if (co2Rows.count > 1)
            {
                // ditto
                bool co2Bit = !(co2OnesSet >= co2ZeroesSet);

                if (co2Bit) {
                    data[i].flip;
                }

                data[i].bitsSet.each!(i => co2Rows[i] = false);
                if (co2Bit) {
                    data[i].flip;
                }
            }
        }
        size_t oxyIndex = oxyRows.bitsSet.takeOne[0];
        size_t co2Index = co2Rows.bitsSet.takeOne[0];
        ulong oxyNumber = data.fold!((a, e) => (a << 1) + e[oxyIndex])(0);
        ulong co2Number = data.fold!((a, e) => (a << 1) + e[co2Index])(0);
        return oxyNumber * co2Number;
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
