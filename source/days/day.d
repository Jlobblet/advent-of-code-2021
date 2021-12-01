module days.day;
import std.file : exists, isFile, readText;
import std.stdio : writefln, writeln;
import timer;

struct Runner
{
    void delegate() runA;
    void delegate() runB;
    this(D)(D day) 
    {
        runA = &day.runA;
        runB = &day.runB;
    }
}

interface Day(TInput, TResult, string path)
{
    final void runA()
    {
        auto t = Timer();
        t.start;
        auto data = parseData(readFile, &t);
        t.lap("Parsing data");
        auto answerA = problemA(data, &t);
        t.stop;
        t.lap("Total");
        writefln("Answer A: %s", answerA);
        t.tabulate.writeln;
        t.reset;
    }

    final void runB()
    {
        auto t = Timer();
        t.start;
        auto data = parseData(readFile, &t);
        t.lap("Parsing data");
        auto answerB = problemB(data, &t);
        t.stop;
        t.lap("Total");
        writefln("Answer B: %s", answerB);
        t.tabulate.writeln;
        t.reset;
    }

    final string readFile()
    in
    {
        assert(path.exists && path.isFile);
    }
    out (r)
    {
        assert(r.length > 0);
    }
    do
    {
        return path.readText;
    }

    TInput parseData(string data, Timer* timer);
    TResult problemA(TInput data, Timer* timer);
    TResult problemB(TInput data, Timer* timer);
}
