module days.day01;

import std.algorithm : count, map, sum;
import std.array : array;
import std.conv : to;
import std.functional : compose;
import std.range : dropBackOne, dropOne, slide, zip, StoppingPolicy;
import std.string : splitLines, strip;
import days.day : Day;
import timer : Timer;

class Day01 : Day!(ulong[], size_t, "bigdata/01.txt")
{
    ulong[] parseData(string data, Timer* timer)
    {
        return data.splitLines.map!(compose!(to!ulong, strip)).array;
    }

    size_t problem(size_t offset)(ulong[] data)
    {
        return StoppingPolicy.shortest.zip(data, data[offset..$]).count!"a[1] > a[0]";
    }

    size_t problemA(ulong[] data, Timer* timer)
    {
        return problem!1(data);
    }

    size_t problemB(ulong[] data, Timer* timer)
    {
        return problem!3(data);
    }
}
