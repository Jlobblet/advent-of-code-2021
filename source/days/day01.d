module days.day01;

import std.algorithm : count, map, sum;
import std.array : array;
import std.conv : to;
import std.functional : compose;
import std.range : dropBackOne, dropOne, slide, zip;
import std.string : splitLines, strip;
import days.day : Day;
import timer : Timer;

class Day01 : Day!(ulong[], size_t, "data/01.txt")
{
    ulong[] parseData(string data, Timer* timer)
    {
        return data.splitLines.map!(compose!(to!ulong, strip)).array;
    }

    size_t problemA(ulong[] data, Timer* timer)
    {
        return zip(data.dropBackOne, data.dropOne).count!"a[1] > a[0]";
    }

    size_t problemB(ulong[] data, Timer* timer)
    {
        auto windows = data.slide(3);
        return zip(windows.save.dropBackOne, windows.save.dropOne).count!"a[1].sum > a[0].sum";
    }
}
