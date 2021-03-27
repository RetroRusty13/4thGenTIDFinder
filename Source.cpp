#include <iostream>
#include <list>

using namespace std;

constexpr unsigned MAG[2] = { 0, 0x9908b0df };

int main()
{
    unsigned pid;
    unsigned hid;
    unsigned lid;
    unsigned tid;

    std::cout << "SYS ASK > Input the Pokemon ID (PID) you want to turn shiny (Hex). Example: 3d50af28" << std::endl << "USER IN < ";
    cin >> hex >> pid;
    std::cout << std::endl << "SYS OUT > PID: " << hex << pid << std::endl;

    hid = pid >> 16;
    lid = pid & 0xffff;

    std::cout << "SYS OUT > HID: " << hid << std::endl;
    std::cout << "SYS OUT > LID: " << lid << std::endl << std::endl;

    std::cout << "SYS ASK > Enter the Trainer ID (TID) you want (Decimal). Example: 42069" << std::endl << "USER IN < ";
    cin >> dec >> tid;

    bool validTID = false;
    if (tid >= 0 && tid <= 65535)
    {
        validTID = true;
    }

    while (!validTID)
    {
        std::cout << std::endl << "SYS ASK > Please re-enter TID. Must be between 0 and 65535" << std::endl << "USER IN < ";
        cin >> dec >> tid;
        if (tid >= 0 && tid <= 65535)
        {
            validTID = true;
            exit;
        }
        if (validTID) exit;
    }
    std::cout << std::endl << "SYS OUT > TID: " << dec << tid << std::endl << std::endl;

    std::cout << "SYS OUT > HID = ";
    for (int i = 0x8000; i != 0; i = i >> 1)
    {
        if ((i & hid) != 0) std::cout << "1";
        else std::cout << "0";
    }

    std::cout << std::endl << "SYS OUT > HID = ";
    for (int i = 0x8000; i != 0; i = i >> 1)
    {
        if ((i & lid) != 0) std::cout << "1";
        else std::cout << "0";
    }

    std::cout << std::endl << "SYS OUT > LID = ";
    for (int i = 0x8000; i != 0; i = i >> 1)
    {
        if ((i & tid) != 0) std::cout << "1";
        else std::cout << "0";
    }
    std::cout << std::endl;

    unsigned sid = ((hid >> 3) ^ (lid >> 3) ^ (tid >> 3)) << 3;

    std::cout << std::endl << "SYS OUT > Possible SIDs:";

    for (int j = 0; j < 8; j++)
    {
        std::cout << std::endl << "SYS OUT > SID = ";
        for (int i = 0x8000; i != 0; i = i >> 1)
        {
            if ((i & sid) != 0) std::cout << "1";
            else std::cout << "0";
        }
        std::cout << " = " << dec << sid;
        sid++;
    }
    sid -= 8;

    unsigned halfways[8];

    for (int j = 0; j < 8; j++)
    {
        unsigned sidtid = (sid << 16) | tid;
        unsigned next = 0;
        unsigned temp = 0;

        for (unsigned i = 0x80000000; i != 0; i = i >> 1)
        {
            next = next | ((sidtid & i) ^ (temp & i));
            temp = next >> 18;
        }

        unsigned step = next; next = 0; temp = 0;
        for (unsigned i = 1; i != 0; i = i << 1)
        {
            next = next | ((step & i) ^ ((temp & i) & (0xefc60000 & i)));
            temp = next << 15;
        }

        step = next; next = 0; temp = 0;
        for (unsigned i = 1; i != 0; i = i << 1)
        {
            next = next | ((step & i) ^ ((temp & i) & (0x9d2c5680 & i)));
            temp = next << 7;
        }

        step = next; next = 0; temp = 0;
        for (unsigned i = 0x80000000; i != 0; i = i >> 1)
        {
            next = next | ((step & i) ^ (temp & i));
            temp = next >> 11;
        }

        halfways[7 - j] = next;
        sid++;
    }

    std::cout << std::endl;

    list<int> seeds;

    std::cout << std::endl << "SYS ASK > If this program previously stopped before it finished, enter the last seed searched to" << std::endl
        << "        > resume where you left off. If not, enter 0 to start searching. If the cmd stops, copy all" << std::endl
        << "        > seeds found and the last # of seeds searched, you will be able to continure from there" << std::endl
                           << "        > after you restart the application." << std::endl << "USER IN < ";
    unsigned resume;
    cin >> resume;

    std::cout << std::endl;
    if (resume != 0) std::cout << "SYS OUT > Resuming at " << resume << "." << std::endl;

    for (unsigned seed = resume; seed < 0xffffffff; seed++)
    {
        unsigned s1 = 0x6c078965 * (seed ^ (seed >> 30)) + 1;
        unsigned s2 = 0x6c078965 * (s1 ^ (s1 >> 30)) + 2;

        unsigned s398 = s2;
        for (uint16_t i = 3; i < 399; i++)
        {
            s398 = 0x6c078965 * (s398 ^ (s398 >> 30)) + i;
        }
        unsigned d = (s1 & 0x80000000) | (s2 & 0x7fffffff);
        s1 = s398 ^ (d >> 1) ^ MAG[d & 1];

        for (int i = 0; i < 8; i++)
        {
            if (s1 == halfways[i])
            {
                unsigned y = s1 ^ (s1 >> 11);
                y ^= ((y << 7) & 0x9d2c5680);
                y ^= ((y << 15) & 0xefc60000);
                y ^= (y >> 18);

                int tid = y & 0xffff;
                int sid = y >> 16;

                std::cout << "SYS OUT > Found seed: ";
                std::cout << hex << seed;
                std::cout << "; TID = ";
                std::cout << dec << tid;
                std::cout << "; SID = ";
                std::cout << dec << sid << std::endl;

                seeds.push_back(seed);
            }
        }

        if (seed % 10000000 == 0)
        {
            if (seed == 0) std::cout << "SYS OUT > Searching seeds - this will take a while..." << std::endl;
            else
            {
                std::cout << "SYS OUT > Seeds searched: ";
                std::cout << seed;
                std::cout << "; ";
                std::cout << ((double)seed) / 0xffffffff * 100;
                std::cout << "% complete." << std::endl;
            }
        }
    }

    unsigned s1 = 0x6c078965 * (0xffffffff ^ (0xffffffff >> 30)) + 1;
    unsigned s2 = 0x6c078965 * (s1 ^ (s1 >> 30)) + 2;

    unsigned s398 = s2;
    for (uint16_t i = 3; i < 399; i++)
    {
        s398 = 0x6c078965 * (s398 ^ (s398 >> 30)) + i;
    }
    unsigned d = (s1 & 0x80000000) | (s2 & 0x7fffffff);
    s1 = s398 ^ (d >> 1) ^ MAG[d & 1];

    for (int i = 0; i < 8; i++)
    {
        if (s1 == halfways[i])
        {
            unsigned y = s1 ^ (s1 >> 11);
            y ^= ((y << 7) & 0x9d2c5680);
            y ^= ((y << 15) & 0xefc60000);
            y ^= (y >> 18);

            int tid = y & 0xffff;
            int sid = y >> 16;

            std::cout << "SYS OUT > Found seed: ";
            std::cout << hex << 0xffffffff;
            std::cout << "; TID = ";
            std::cout << dec << tid;
            std::cout << "; SID = ";
            std::cout << dec << sid << std::endl;

            seeds.push_back(0xffffffff);
        }
    }

    std::cout << "SYS OUT > Seeds searched: 4294967296; 100% complete." << std::endl << "SYS OUT > Done!" << std::endl;

    bool missedseeds;
    std::cout << std::endl << "SYS ASK > If the cmd previously stopped and you had to restart, did it output any seeds? (1 = yes, 0 = no)" << std::endl << "USER IN < ";
    std::cin >> missedseeds;

    unsigned missed;
    while (missedseeds)
    {
        std::cout << std::endl << "SYS ASK > Enter a seed the cmd found before it stopped (Hex)." << std::endl << "USER IN < ";
        std::cin >> hex >> missed;
        seeds.push_back(missed);

        std::cout << std::endl << "SYS ASK > Are there any more seeds that were skipped? (1 = yes, 0 = no)" << std::endl << "USER IN < ";
        std::cin >> missedseeds;
    }

    std::cout << std::endl << "SYS OUT > Seeds Found: " << seeds.size() << std::endl;

    int month;
    int day;
    int hour;
    int minute;
    unsigned delay;

    for (unsigned seed : seeds)
    {
        unsigned s1 = 0x6c078965 * (seed ^ (seed >> 30)) + 1;
        unsigned s2 = 0x6c078965 * (s1 ^ (s1 >> 30)) + 2;

        unsigned s398 = s2;
        for (uint16_t i = 3; i < 399; i++)
        {
            s398 = 0x6c078965 * (s398 ^ (s398 >> 30)) + i;
        }
        unsigned d = (s1 & 0x80000000) | (s2 & 0x7fffffff);
        s1 = s398 ^ (d >> 1) ^ MAG[d & 1];

        unsigned y = s1 ^ (s1 >> 11);
        y ^= ((y << 7) & 0x9d2c5680);
        y ^= ((y << 15) & 0xefc60000);
        y ^= (y >> 18);

        tid = y & 0xffff;
        sid = y >> 16;

        if ((seed >> 24) > 24) day = 25;
        else day = seed >> 24;

        month = max((int) (((seed & 0xff000000) >> 24) / day), 1);
        minute = ((seed & 0xff000000) >> 24) - day * month;
        hour = min((int) ((seed & 0xff0000) >> 16), 23);
        delay = (seed & 0xffffff) - (hour << 16) - 20;

        if (seed >> 24 == 0)
        {
            day = 25;
            month = 10;
            minute = 6;
        }

        std::cout
            << "SYS OUT > Seed: " << hex << seed << std::endl
            << "        > TID: " << dec << tid << std::endl
            << "        > SID: " << dec << sid << std::endl
            << "        > Year: 2020" << std::endl
            << "        > Month: " << dec << month << std::endl
            << "        > Day: " << dec << day << std::endl
            << "        > Hour: " << dec << hour << std::endl
            << "        > Minute: " << dec << minute << std::endl
            << "        > Second: 0" << std::endl
            << "        > Delay: " << dec << delay << std::endl << std::endl;
    }

    std::cout << std::endl << std::endl << std::endl << "SYS OUT > ";
    system("pause");
}