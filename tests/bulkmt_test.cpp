#define BOOST_TEST_MODULE test_main
#include <boost/test/unit_test.hpp>
#include <boost/mpl/assert.hpp>

#include "../src/Handler.h"
#include "../src/Writers.h"

using Commands = std::vector<std::string>;


BOOST_AUTO_TEST_SUITE(test_bulk)

    BOOST_AUTO_TEST_CASE(example_1)
    {
        std::stringbuf out_buffer;
        std::ostream out_stream(&out_buffer);

        auto handler = std::make_shared<Handler>();
        auto consoleWriter = std::make_shared<ConsoleWriter>(out_stream);
        auto fileWriter = std::make_shared<FileWriter>();
        consoleWriter->subscribe(handler);
        fileWriter->subscribe(handler);

        handler->setN(3);
        handler->addCommand("cmd1");
        handler->addCommand("cmd2");
        handler->addCommand("cmd3");

        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        std::ifstream file{fileWriter->getName()};
        std::stringstream string_stream;
        string_stream << file.rdbuf();
        file.close();
        std::remove(fileWriter->getName().c_str());

        BOOST_CHECK_EQUAL(out_buffer.str(),"bulk: cmd1, cmd2, cmd3\n");
        BOOST_CHECK_EQUAL(string_stream.str(),"bulk: cmd1, cmd2, cmd3");

        out_buffer.str("");
        string_stream.str("");

        handler->addCommand("cmd4");
        handler->addCommand("cmd5");
        handler->stop();

        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        file.open(fileWriter->getName());
        string_stream << file.rdbuf();
        file.close();
        std::remove(fileWriter->getName().c_str());

        BOOST_CHECK_EQUAL(out_buffer.str(),"bulk: cmd4, cmd5\n");
        BOOST_CHECK_EQUAL(string_stream.str(),"bulk: cmd4, cmd5");
    }

////////////////////////////////////////////////////////////////////////////////////////////////

    BOOST_AUTO_TEST_CASE(example_2)
    {
        std::stringbuf out_buffer;
        std::ostream out_stream(&out_buffer);

        auto handler = std::make_shared<Handler>();
        auto consoleWriter = std::make_shared<ConsoleWriter>(out_stream);
        auto fileWriter = std::make_shared<FileWriter>();
        consoleWriter->subscribe(handler);
        fileWriter->subscribe(handler);
        
        handler->setN(3);
        handler->addCommand("cmd1");
        handler->addCommand("cmd2");
        handler->addCommand("cmd3");

        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        std::ifstream file{fileWriter->getName()};
        std::stringstream string_stream;
        string_stream << file.rdbuf();
        file.close();
        std::remove(fileWriter->getName().c_str());

        BOOST_CHECK_EQUAL(out_buffer.str(),"bulk: cmd1, cmd2, cmd3\n");
        BOOST_CHECK_EQUAL(string_stream.str(),"bulk: cmd1, cmd2, cmd3");

        out_buffer.str("");
        string_stream.str("");

        handler->addCommand("{");
        handler->addCommand("cmd4");
        handler->addCommand("cmd5");
        handler->addCommand("cmd6");
        handler->addCommand("cmd7");
        handler->addCommand("}");

        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        file.open(fileWriter->getName());
        string_stream << file.rdbuf();
        file.close();
        std::remove(fileWriter->getName().c_str());

        BOOST_CHECK_EQUAL(out_buffer.str(),"bulk: cmd4, cmd5, cmd6, cmd7\n");
        BOOST_CHECK_EQUAL(string_stream.str(),"bulk: cmd4, cmd5, cmd6, cmd7");
    }

////////////////////////////////////////////////////////////////////////////////////////////////

    BOOST_AUTO_TEST_CASE(example_3)
    {
        std::stringbuf out_buffer;
        std::ostream out_stream(&out_buffer);

        auto handler = std::make_shared<Handler>();
        auto consoleWriter = std::make_shared<ConsoleWriter>(out_stream);
        auto fileWriter = std::make_shared<FileWriter>();
        consoleWriter->subscribe(handler);
        fileWriter->subscribe(handler);

        handler->setN(1);
        handler->addCommand("{");
        handler->addCommand("cmd1");
        handler->addCommand("cmd2");
        handler->addCommand("{");
        handler->addCommand("cmd3");
        handler->addCommand("cmd4");
        handler->addCommand("}");
        handler->addCommand("cmd5");
        handler->addCommand("cmd6");
        handler->addCommand("}");

        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        std::ifstream file{fileWriter->getName()};
        std::stringstream string_stream;
        string_stream << file.rdbuf();
        file.close();
        std::remove(fileWriter->getName().c_str());

        BOOST_CHECK_EQUAL(out_buffer.str(),"bulk: cmd1, cmd2, cmd3, cmd4, cmd5, cmd6\n");
        BOOST_CHECK_EQUAL(string_stream.str(),"bulk: cmd1, cmd2, cmd3, cmd4, cmd5, cmd6");
    }

////////////////////////////////////////////////////////////////////////////////////////////////

    BOOST_AUTO_TEST_CASE(example_4)
    {
        std::stringbuf out_buffer;
        std::ostream out_stream(&out_buffer);

        auto handler = std::make_shared<Handler>();
        auto consoleWriter = std::make_shared<ConsoleWriter>(out_stream);
        auto fileWriter = std::make_shared<FileWriter>();
        consoleWriter->subscribe(handler);
        fileWriter->subscribe(handler);

        handler->setN(4);
        handler->addCommand("cmd1");
        handler->addCommand("cmd2");
        handler->addCommand("cmd3");
        handler->addCommand("{");
        auto name = fileWriter->getName();
        handler->addCommand("cmd4");
        handler->addCommand("cmd5");
        handler->addCommand("cmd6");
        handler->addCommand("cmd7");
        handler->stop();

        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        std::ifstream file{name};
        std::stringstream string_stream;
        string_stream << file.rdbuf();
        file.close();
        std::remove(name.c_str());

        BOOST_CHECK_EQUAL(out_buffer.str(),"bulk: cmd1, cmd2, cmd3\n");
        BOOST_CHECK_EQUAL(string_stream.str(),"bulk: cmd1, cmd2, cmd3");

        name = fileWriter->getName();
        file.open(name);
        BOOST_CHECK(!file.is_open());
    }

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(test_bulkmt)

    BOOST_AUTO_TEST_CASE(two_file_friters)
    {
        auto main_counter = std::make_shared<Counter>("main");
        auto file_counter1 = std::make_shared<Counter>("file1");
        auto file_counter2 = std::make_shared<Counter>("file2");

        auto handler = std::make_shared<Handler>();
        auto fileWriter1 = std::make_shared<FileWriter>();
        auto fileWriter2 = std::make_shared<FileWriter>();
        fileWriter1->subscribe(handler);
        fileWriter2->subscribe(handler);

        handler->setCounter(main_counter);
        fileWriter1->setCounter(file_counter1);
        fileWriter2->setCounter(file_counter2);

        handler->setN(1);
        handler->addCommand("cmd1");
        std::remove(fileWriter1->getName().c_str());
        std::remove(fileWriter2->getName().c_str());
        handler->addCommand("cmd2");
        std::remove(fileWriter1->getName().c_str());
        std::remove(fileWriter2->getName().c_str());
        handler->addCommand("cmd3");
        std::remove(fileWriter1->getName().c_str());
        std::remove(fileWriter2->getName().c_str());

        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        int lines_c,commands_c,blocks_c;
        std::tie(lines_c,commands_c,blocks_c) = main_counter->get();
        BOOST_CHECK_EQUAL(lines_c,3);
        BOOST_CHECK_EQUAL(commands_c,3);
        BOOST_CHECK_EQUAL(blocks_c,3);

        int commands_c2,blocks_c2;
        std::tie(lines_c,commands_c,blocks_c) = file_counter1->get();
        std::tie(lines_c,commands_c2,blocks_c2) = file_counter2->get();
        BOOST_CHECK_EQUAL(commands_c + commands_c2,3);
        BOOST_CHECK_EQUAL(blocks_c + blocks_c2,3);
    }

////////////////////////////////////////////////////////////////////////////////////////////////
   
    BOOST_AUTO_TEST_CASE(repeat)
    {
        std::stringbuf out_buffer;
        std::ostream out_stream(&out_buffer);

        auto handler = std::make_shared<Handler>();
        auto fileWriter1 = std::make_shared<FileWriter>();
        auto fileWriter2 = std::make_shared<FileWriter>();
        auto console_writer = std::make_shared<ConsoleWriter>(out_stream);
        fileWriter1->subscribe(handler);
        fileWriter2->subscribe(handler);
        console_writer->subscribe(handler);
#ifdef ALL_TESTS
        std::cout << "Starting a recurring test (" << N_REPEAT <<"): test_bulkmt - repeat " << "\n";
        for (auto i = 0; i < N_REPEAT; i++)
#endif
        {
            auto main_counter = std::make_shared<Counter>("main");
            auto log_counter = std::make_shared<Counter>("log");
            auto file_counter1 = std::make_shared<Counter>("file1");
            auto file_counter2 = std::make_shared<Counter>("file2");
            handler->setCounter(main_counter);
            console_writer->setCounter(log_counter);
            fileWriter1->setCounter(file_counter1);
            fileWriter2->setCounter(file_counter2);

            handler->setN(2);
            handler->addCommand("cmd1");
            handler->addCommand("cmd2");
            std::remove(fileWriter1->getName().c_str());
            std::remove(fileWriter2->getName().c_str());
            handler->addCommand("cmd3");
            handler->addCommand("{");
            std::remove(fileWriter1->getName().c_str());
            std::remove(fileWriter2->getName().c_str());
            handler->addCommand("cmd4");
            handler->addCommand("cmd5");
            handler->addCommand("cmd6");
            handler->addCommand("}");
            std::remove(fileWriter1->getName().c_str());
            std::remove(fileWriter2->getName().c_str());
            handler->addCommand("cmd7");
            handler->stop();
            std::remove(fileWriter1->getName().c_str());
            std::remove(fileWriter2->getName().c_str());
#ifdef ALL_TESTS
            std::cout << i << ":" << N_REPEAT << " (test_bulkmt - repeat)\n";
#endif
            int lines_c,commands_c,blocks_c;
            std::tie(lines_c,commands_c,blocks_c) = main_counter->get();
            BOOST_CHECK_EQUAL(lines_c,9);
            BOOST_CHECK_EQUAL(commands_c,7);
            BOOST_CHECK_EQUAL(blocks_c,4);

            std::tie(lines_c,commands_c,blocks_c) = log_counter->get();
            BOOST_CHECK_EQUAL(commands_c,7);
            BOOST_CHECK_EQUAL(blocks_c,4);

            int commands_c2,blocks_c2;
            std::tie(lines_c,commands_c,blocks_c) = file_counter1->get();
            std::tie(lines_c,commands_c2,blocks_c2) = file_counter2->get();
            BOOST_CHECK_EQUAL(commands_c + commands_c2,7);
            BOOST_CHECK_EQUAL(blocks_c + blocks_c2,4);
        }
    }

////////////////////////////////////////////////////////////////////////////////////////////////

    BOOST_AUTO_TEST_CASE(many_writers)
    {
        std::stringbuf out_buffer;
        std::ostream out_stream(&out_buffer);

        auto handler = std::make_shared<Handler>();
        auto file_writer1 = std::make_shared<FileWriter>();
        auto file_writer2 = std::make_shared<FileWriter>();
        auto file_writer3 = std::make_shared<FileWriter>();
        auto file_writer4 = std::make_shared<FileWriter>();
        auto file_writer5 = std::make_shared<FileWriter>();
        auto console_writer1 = std::make_shared<ConsoleWriter>(out_stream);
        auto console_writer2 = std::make_shared<ConsoleWriter>(out_stream);
        auto console_writer3 = std::make_shared<ConsoleWriter>(out_stream);
        auto console_writer4 = std::make_shared<ConsoleWriter>(out_stream);
        auto console_writer5 = std::make_shared<ConsoleWriter>(out_stream);
        file_writer1->subscribe(handler);
        file_writer2->subscribe(handler);
        file_writer3->subscribe(handler);
        file_writer4->subscribe(handler);
        file_writer5->subscribe(handler);
        console_writer1->subscribe(handler);
        console_writer2->subscribe(handler);
        console_writer3->subscribe(handler);
        console_writer4->subscribe(handler);
        console_writer5->subscribe(handler);

        auto main_counter = std::make_shared<Counter>("main");
        auto log_counter1 = std::make_shared<Counter>("log1");
        auto log_counter2 = std::make_shared<Counter>("log2");
        auto log_counter3 = std::make_shared<Counter>("log3");
        auto log_counter4 = std::make_shared<Counter>("log4");
        auto log_counter5 = std::make_shared<Counter>("log5");
        auto file_counter1 = std::make_shared<Counter>("file1");
        auto file_counter2 = std::make_shared<Counter>("file2");
        auto file_counter3 = std::make_shared<Counter>("file3");
        auto file_counter4 = std::make_shared<Counter>("file4");
        auto file_counter5 = std::make_shared<Counter>("file5");
        handler->setCounter(main_counter);
        console_writer1->setCounter(log_counter1);
        console_writer2->setCounter(log_counter2);
        console_writer3->setCounter(log_counter3);
        console_writer4->setCounter(log_counter4);
        console_writer5->setCounter(log_counter5);
        file_writer1->setCounter(file_counter1);
        file_writer2->setCounter(file_counter2);
        file_writer3->setCounter(file_counter3);
        file_writer4->setCounter(file_counter4);
        file_writer5->setCounter(file_counter5);

        handler->setN(2);
        handler->addCommand("cmd1");
        handler->addCommand("cmd2");
        std::remove(file_writer1->getName().c_str());
        std::remove(file_writer2->getName().c_str());
        std::remove(file_writer3->getName().c_str());
        std::remove(file_writer4->getName().c_str());
        std::remove(file_writer5->getName().c_str());
        handler->addCommand("cmd3");
        handler->addCommand("{");
        std::remove(file_writer1->getName().c_str());
        std::remove(file_writer2->getName().c_str());
        std::remove(file_writer3->getName().c_str());
        std::remove(file_writer4->getName().c_str());
        std::remove(file_writer5->getName().c_str());
        handler->addCommand("cmd4");
        handler->addCommand("cmd5");
        handler->addCommand("cmd6");
        handler->addCommand("}");
        std::remove(file_writer1->getName().c_str());
        std::remove(file_writer2->getName().c_str());
        std::remove(file_writer3->getName().c_str());
        std::remove(file_writer4->getName().c_str());
        std::remove(file_writer5->getName().c_str());
        handler->addCommand("cmd7");
        handler->stop();
        std::remove(file_writer1->getName().c_str());
        std::remove(file_writer2->getName().c_str());
        std::remove(file_writer3->getName().c_str());
        std::remove(file_writer4->getName().c_str());
        std::remove(file_writer5->getName().c_str());

        int lines_c,commands_c,blocks_c;
        std::tie(lines_c,commands_c,blocks_c) = main_counter->get();
        BOOST_CHECK_EQUAL(lines_c,9);
        BOOST_CHECK_EQUAL(commands_c,7);
        BOOST_CHECK_EQUAL(blocks_c,4);

        std::tie(lines_c,commands_c,blocks_c) = log_counter1->get();
        BOOST_CHECK_EQUAL(commands_c,7);
        BOOST_CHECK_EQUAL(blocks_c,4);

        std::tie(lines_c,commands_c,blocks_c) = log_counter2->get();
        BOOST_CHECK_EQUAL(commands_c,7);
        BOOST_CHECK_EQUAL(blocks_c,4);

        std::tie(lines_c,commands_c,blocks_c) = log_counter3->get();
        BOOST_CHECK_EQUAL(commands_c,7);
        BOOST_CHECK_EQUAL(blocks_c,4);

        std::tie(lines_c,commands_c,blocks_c) = log_counter4->get();
        BOOST_CHECK_EQUAL(commands_c,7);
        BOOST_CHECK_EQUAL(blocks_c,4);

        std::tie(lines_c,commands_c,blocks_c) = log_counter5->get();
        BOOST_CHECK_EQUAL(commands_c,7);
        BOOST_CHECK_EQUAL(blocks_c,4);

        int commands_c2,blocks_c2;
        std::tie(lines_c,commands_c,blocks_c) = file_counter1->get();
        std::tie(lines_c,commands_c2,blocks_c2) = file_counter2->get();
        commands_c += commands_c2;
        blocks_c += blocks_c2;

        std::tie(lines_c,commands_c2,blocks_c2) = file_counter3->get();
        commands_c += commands_c2;
        blocks_c += blocks_c2;

        std::tie(lines_c,commands_c2,blocks_c2) = file_counter4->get();
        commands_c += commands_c2;
        blocks_c += blocks_c2;
        
        std::tie(lines_c,commands_c2,blocks_c2) = file_counter5->get();
        commands_c += commands_c2;
        blocks_c += blocks_c2;

        BOOST_CHECK_EQUAL(commands_c,7);
        BOOST_CHECK_EQUAL(blocks_c,4);
    }

BOOST_AUTO_TEST_SUITE_END()