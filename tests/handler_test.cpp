#include <boost/test/unit_test.hpp>
#include <boost/mpl/assert.hpp>

#include "Handler.h"
#include "Writers.h"

using Commands = std::vector<std::string>;

BOOST_AUTO_TEST_SUITE(test_handler)

    BOOST_AUTO_TEST_CASE(print_commands)
    {
        std::stringbuf out_buffer;
        std::ostream out_stream(&out_buffer);

        auto handler = std::make_shared<Handler>(2);
        auto consoleWriter = std::make_shared<ConsoleWriter>(out_stream);
        auto fileWriter = std::make_shared<FileWriter>();
        consoleWriter->subscribe(handler);
        fileWriter->subscribe(handler);
        handler->addCommand("cmd1");
        handler->addCommand("cmd2");

        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        std::ifstream file{fileWriter->getName()};
        std::stringstream string_stream;
        string_stream << file.rdbuf();
        file.close();
        std::remove(fileWriter->getName().c_str());

        BOOST_CHECK_EQUAL(out_buffer.str(),"bulk: cmd1, cmd2\n");
        BOOST_CHECK_EQUAL(string_stream.str(),"bulk: cmd1, cmd2");
    }

////////////////////////////////////////////////////////////////////////////////////////////////

    BOOST_AUTO_TEST_CASE(print_blocks)
    {
        std::stringbuf out_buffer;
        std::ostream out_stream(&out_buffer);

        auto handler = std::make_shared<Handler>(5);
        auto consoleWriter = std::make_shared<ConsoleWriter>(out_stream);
        auto fileWriter = std::make_shared<FileWriter>();
        consoleWriter->subscribe(handler);
        fileWriter->subscribe(handler);

        handler->addCommand("cmd1");
        handler->addCommand("cmd2");
        handler->addCommand("{");

        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        std::ifstream file{fileWriter->getName()};
        std::stringstream string_stream;
        string_stream << file.rdbuf();
        file.close();
        std::remove(fileWriter->getName().c_str());

        BOOST_CHECK_EQUAL(out_buffer.str(),"bulk: cmd1, cmd2\n");
        BOOST_CHECK_EQUAL(string_stream.str(),"bulk: cmd1, cmd2");

        out_buffer.str("");
        string_stream.str("");

        handler->addCommand("cmd3");
        handler->addCommand("cmd4");
        handler->addCommand("}");

        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        file.open(fileWriter->getName());
        string_stream << file.rdbuf();
        file.close();
        std::remove(fileWriter->getName().c_str());

        BOOST_CHECK_EQUAL(out_buffer.str(),"bulk: cmd3, cmd4\n");
        BOOST_CHECK_EQUAL(string_stream.str(),"bulk: cmd3, cmd4");
    }

////////////////////////////////////////////////////////////////////////////////////////////////

    BOOST_AUTO_TEST_CASE(delete_writer)
    {
        std::stringbuf out_buffer;
        std::ostream out_stream(&out_buffer);

        auto handler = std::make_shared<Handler>(1);
        {
            auto fileWriter = std::make_shared<FileWriter>();
            fileWriter->subscribe(handler);
        }
        auto consoleWriter = std::make_shared<ConsoleWriter>(out_stream);
        consoleWriter->subscribe(handler);
        
        handler->addCommand("cmd1");
        handler->stop();

        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        BOOST_CHECK_EQUAL(out_buffer.str(),"bulk: cmd1\n");
    }

////////////////////////////////////////////////////////////////////////////////////////////////

    BOOST_AUTO_TEST_CASE(delete_run_writer)
    {
#ifdef ALL_TESTS
        std::cout << "Starting a recurring test (" << N_DELETED << "): test_handler - delete_run_writer" << "\n";
        for (auto i = 0; i < N_DELETED; i++) 
#endif
        {
            std::stringbuf out_buffer;
            std::ostream out_stream(&out_buffer);

            auto main_counter = std::make_shared<Counter>("main");
            auto log_counter = std::make_shared<Counter>("log");
            auto file_counter = std::make_shared<Counter>("file");
            auto handler = std::make_shared<Handler>(2);
            auto consoleWriter = std::make_shared<ConsoleWriter>(out_stream);
            consoleWriter->subscribe(handler);
            handler->setCounter(main_counter);
            consoleWriter->setCounter(log_counter);
            std::string name;
            {
                auto fileWriter = std::make_shared<FileWriter>();
                fileWriter->subscribe(handler);
                fileWriter->setCounter(file_counter);
                handler->addCommand("cmd1");
                handler->addCommand("cmd2");
                name = fileWriter->getName();
            }
            handler->addCommand("cmd3");
            handler->addCommand("cmd4");
            handler->addCommand("cmd5");
            handler->stop();

            std::this_thread::sleep_for(std::chrono::milliseconds(10));

            std::fstream file{name};
            std::stringstream string_stream;
            string_stream << file.rdbuf();
            file.close();
            std::remove(name.c_str());
#ifdef ALL_TESTS
        std::cout << i << ":" << N_DELETED << " (test_handler - delete_run_writer)\n";
#endif
            BOOST_CHECK_EQUAL(out_buffer.str(),"bulk: cmd1, cmd2\nbulk: cmd3, cmd4\nbulk: cmd5\n");
            BOOST_CHECK_EQUAL(string_stream.str(),"bulk: cmd1, cmd2");

            int lines_c,commands_c,blocks_c;
            std::tie(lines_c,commands_c,blocks_c) = main_counter->get();
            BOOST_CHECK_EQUAL(lines_c,5);
            BOOST_CHECK_EQUAL(commands_c,5);
            BOOST_CHECK_EQUAL(blocks_c,3);

            std::tie(lines_c,commands_c,blocks_c) = log_counter->get();
            BOOST_CHECK_EQUAL(commands_c,5);
            BOOST_CHECK_EQUAL(blocks_c,3);

            std::tie(lines_c,commands_c,blocks_c) = file_counter->get();
            BOOST_CHECK_EQUAL(commands_c,2);
            BOOST_CHECK_EQUAL(blocks_c,1);
        }
    }

////////////////////////////////////////////////////////////////////////////////////////////////

    BOOST_AUTO_TEST_CASE(delete_handler)
    {
#ifdef ALL_TESTS
        std::cout << "Starting a recurring test (" << N_DELETED << "): test_handler - delete_handler" << "\n";
        for (auto i = 0; i < N_DELETED; i++) 
#endif
        {
            std::stringstream out_stream1;
            std::stringstream out_stream2;

            auto main_counter = std::make_shared<Counter>("main");
            auto log_counter1 = std::make_shared<Counter>("log1");
            auto log_counter2 = std::make_shared<Counter>("log2");
            auto console_writer1 = std::make_shared<ConsoleWriter>(out_stream1);
            auto console_writer2 = std::make_shared<ConsoleWriter>(out_stream2);           
            console_writer1->setCounter(log_counter1);
            console_writer2->setCounter(log_counter2);
            {
                auto handler = std::make_shared<Handler>(1);
                handler->setCounter(main_counter);
                console_writer1->subscribe(handler);
                console_writer2->subscribe(handler);
                handler->addCommand("cmd1");
                handler->addCommand("cmd2");
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(10));

#ifdef ALL_TESTS
        std::cout << i << ":" << N_DELETED << " (test_handler - delete_handler)\n";
#endif

            int lines_c,commands_c,blocks_c;
            std::tie(lines_c,commands_c,blocks_c) = main_counter->get();
            BOOST_CHECK_EQUAL(lines_c,2);
            BOOST_CHECK_EQUAL(commands_c,2);
            BOOST_CHECK_EQUAL(blocks_c,2);

            std::tie(lines_c,commands_c,blocks_c) = log_counter1->get();
            BOOST_CHECK_EQUAL(commands_c,2);
            BOOST_CHECK_EQUAL(blocks_c,2);

            std::tie(lines_c,commands_c,blocks_c) = log_counter2->get();
            BOOST_CHECK_EQUAL(commands_c,2);
            BOOST_CHECK_EQUAL(blocks_c,2);
        }
    }

////////////////////////////////////////////////////////////////////////////////////////////////

    BOOST_AUTO_TEST_CASE(large_string)
    {
        Handler handler(4);
        std::string command("123456789012345678901234567890123456789012345678901");
        assert(command.size() == 51);
        BOOST_CHECK_THROW(handler.addCommand(command),std::exception);
    }

////////////////////////////////////////////////////////////////////////////////////////////////

    BOOST_AUTO_TEST_CASE(error_set_N)
    {
        BOOST_CHECK_THROW(Handler handler(-1),std::exception);
    }

////////////////////////////////////////////////////////////////////////////////////////////////

    BOOST_AUTO_TEST_CASE(empty_string)
    {
        std::stringbuf out_buffer;
        std::ostream out_stream(&out_buffer);

        auto handler = std::make_shared<Handler>(2);
        auto consoleWriter = std::make_shared<ConsoleWriter>(out_stream);
        auto fileWriter = std::make_shared<FileWriter>();
        consoleWriter->subscribe(handler);
        fileWriter->subscribe(handler);
        
        handler->addCommand("");
        handler->addCommand("");
        handler->stop();

        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        std::fstream file;
        file.open(fileWriter->getName());
        std::stringstream string_stream;
        string_stream << file.rdbuf();
        file.close();
        std::remove(fileWriter->getName().c_str());

        BOOST_CHECK_EQUAL(out_buffer.str(),"bulk: , \n");
        BOOST_CHECK_EQUAL(out_buffer.str(),"bulk: , \n"); 
    }

////////////////////////////////////////////////////////////////////////////////////////////////

    BOOST_AUTO_TEST_CASE(start_empty_block)
    {
        std::stringbuf out_buffer;
        std::ostream out_stream(&out_buffer);

        auto handler = std::make_shared<Handler>(2);
        auto consoleWriter = std::make_shared<ConsoleWriter>(out_stream);
        auto fileWriter = std::make_shared<FileWriter>();
        consoleWriter->subscribe(handler);
        fileWriter->subscribe(handler);
        
        handler->addCommand("{");
        handler->stop();

        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        std::fstream file;
        file.open(fileWriter->getName());
        std::stringstream string_stream;
        string_stream << file.rdbuf();
        file.close();

        BOOST_CHECK(out_buffer.str().empty());
        BOOST_CHECK(out_buffer.str().empty()); 
    }

////////////////////////////////////////////////////////////////////////////////////////////////

    BOOST_AUTO_TEST_CASE(empty_block)
    {
        std::stringbuf out_buffer;
        std::ostream out_stream(&out_buffer);

        auto handler = std::make_shared<Handler>(2);
        auto consoleWriter = std::make_shared<ConsoleWriter>(out_stream);
        consoleWriter->subscribe(handler);
        
        handler->addCommand("cmd1");
        handler->addCommand("{");
        BOOST_CHECK_THROW(handler->addCommand("}"),std::exception);
    }

BOOST_AUTO_TEST_SUITE_END()
