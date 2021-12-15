module timer;

import core.time : Duration, MonoTime, MonoTimeImpl;
import std.algorithm : map, max, maxElement, sum;
import std.array : Appender, array, appender;
import std.conv : to;
import std.format : format;
import std.range : repeat, walkLength, zip;
import std.string : leftJustify, strip;
import std.uni : byGrapheme;

string formatDur(Duration d)
{
    auto s = appender!string;
    auto splitTime = d.split!("seconds", "msecs", "usecs", "nsecs");
    auto units = ["s", "ms", "us", "ns"];
    auto components = [
        splitTime.seconds, splitTime.msecs, splitTime.usecs, splitTime.nsecs
    ];
    bool leading = true;
    foreach (tup; components.zip(units))
    {
        if (leading && tup[0] == 0)
        {
            continue;
        }
        leading = false;
        s ~= tup[0].to!string;
        s ~= tup[1];
        s ~= ' ';
    }
    return s[].strip;
}

struct Timer
{
    private wstring[] headers = ["Label", "Time"];
    private Appender!(wstring[]) labels = appender!(wstring[]);
    private Appender!(MonoTime[]) times = appender!(MonoTime[]);
    private MonoTime startTime;
    private MonoTime stopTime;

    @nogc public void start()
    {
        startTime = MonoTime.currTime;
    }

    public void lap(wstring label)
    {
        times ~= MonoTime.currTime;
        labels ~= label;
    }

    @nogc public void stop()
    {
        stopTime = MonoTime.currTime;
    }

    public void reset()
    {
        times.clear;
        labels.clear;
    }

    public wstring tabulate()
    {
        auto ts = [startTime] ~ times[];
        wstring[] stringTimes = ts[0 .. $ - 1].zip(ts[1 .. $])
            .map!(tup => (tup[1] - tup[0]).formatDur.to!wstring).array;
        wstring[] summary = [
            "Total", (stopTime - startTime).formatDur.to!wstring
        ];
        ulong[] columnWidths = headers.zip([labels[], stringTimes], summary)
            .map!(tup => max(tup[0].byGrapheme.walkLength,
                    tup[1].map!(a => a.byGrapheme.walkLength).maxElement,
                    tup[2].byGrapheme.walkLength) + 2).array;
        ulong totalLineWidth = columnWidths.sum + columnWidths.length + 2;
        ulong rows = labels[].length + 6;

        void makeRow(Appender!wstring* appender, wchar left, wstring col1,
                wchar middle, wstring col2, wchar right)
        {
            appender.reserve(totalLineWidth);
            *appender ~= left;
            *appender ~= col1;
            *appender ~= middle;
            *appender ~= col2;
            *appender ~= right;
            *appender ~= '\n';
        }

        void makePaddedRow(Appender!wstring* appender, wchar left, wstring col1,
                wchar middle, wstring col2, wchar right)
        {
            makeRow(appender, left, ' ' ~ col1.leftJustify(columnWidths[0] - 1),
                    middle, ' ' ~ col2.leftJustify(columnWidths[1] - 1), right);
        }

        void makeDividerRow(Appender!wstring* appender, wchar left, wchar col,
                wchar middle, wchar right)
        {
            makeRow(appender, left, col.repeat(columnWidths[0]).array, middle,
                    col.repeat(columnWidths[1]).array, right);
        }

        auto tableBuilder = appender!wstring;
        tableBuilder.reserve(rows * totalLineWidth);
        // Top row
        auto lineBuilder = appender!wstring;
        makeDividerRow(&lineBuilder, '╔', '═', '╤', '╗');
        tableBuilder ~= lineBuilder[];

        // Headers
        // You can't clear a string appender (because strings are immutable) so replace it instead
        lineBuilder = appender!wstring;
        makePaddedRow(&lineBuilder, '║', headers[0], '│', headers[1], '║');
        tableBuilder ~= lineBuilder[];

        // Divider
        lineBuilder = appender!wstring;
        makeDividerRow(&lineBuilder, '╟', '─', '┼', '╢');
        tableBuilder ~= lineBuilder[];

        // Labels
        foreach (tup; labels[].zip(stringTimes))
        {
            lineBuilder = appender!wstring;
            makePaddedRow(&lineBuilder, '║', tup[0], '│', tup[1], '║');
            tableBuilder ~= lineBuilder[];
        }

        // Divider
        lineBuilder = appender!wstring;
        makeDividerRow(&lineBuilder, '╟', '─', '┼', '╢');
        tableBuilder ~= lineBuilder[];

        // Total time
        lineBuilder = appender!wstring;
        makePaddedRow(&lineBuilder, '║', summary[0], '│', summary[1], '║');
        tableBuilder ~= lineBuilder[];

        // Bottom
        lineBuilder = appender!wstring;
        makeDividerRow(&lineBuilder, '╚', '═', '╧', '╝');
        tableBuilder ~= lineBuilder[];

        return tableBuilder[];
    }
}
