import std.conv : to;
import std.regex: ctRegex, matchFirst;
import std.stdio : readln, stdout, writef, writeln;
import std.string : icmp, strip;
import days.day : Runner;
import days.days;

void main()
{
    // Add a reference to each day here
    // scope Runner[] days = [Runner(new Day01)], Runner(new Day02), ...];
    scope Runner[] days = [];
    auto length = days.length;
    if (length == 0)
    {
        writeln("No days present!");
        return;
    }
    auto r = ctRegex!(r"^(\d+)([ab])$", "s");
    while (true)
    {
        writef("Enter a day to run (1 - %s) followed by 'a' or 'b', or 'q' to quit: ", length);
        stdout.flush;
        auto input = readln.strip;
        if (input == "q")
        {
            break;
        }
        else
        {
            auto m = input.matchFirst(r);
            if (m.empty) { continue; }
            auto index = m[1].to!ulong - 1;
            auto runner = days[index];
            if (m[2].icmp("a") == 0)
            {
                runner.runA();
            }
            else
            {
                runner.runB();
            }
        }
    }
}
