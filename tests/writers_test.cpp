#include <boost/test/unit_test.hpp>
#include <boost/mpl/assert.hpp>

#include "../src/Writers.h"
#include "helper.h"

using Commands = std::vector<std::string>;


BOOST_AUTO_TEST_SUITE(test_writers)

    BOOST_AUTO_TEST_CASE(print_console)
    {
        std::stringbuf out_buffer;
        std::ostream out_stream(&out_buffer);
        ConsoleWriter writer(out_stream);
        auto commands = std::make_shared<Commands>(Commands{"cmd1", "cmd2"});
        BOOST_CHECK_NO_THROW(writer.update(commands));
        BOOST_CHECK_NO_THROW(writer.print());
        BOOST_CHECK_EQUAL(out_buffer.str(),"bulk: cmd1, cmd2\n");
    }

////////////////////////////////////////////////////////////////////////////////////////////////

    BOOST_AUTO_TEST_CASE(print_file)
    {
        auto time = std::make_shared<std::time_t>();
        FileWriter writer(time,1);
        auto commands = std::make_shared<Commands>(Commands{"cmd1"});
        BOOST_CHECK_NO_THROW(writer.update(commands));
        BOOST_CHECK_NO_THROW(writer.print());
        std::ifstream file{name(time)};
        std::stringstream string_stream;
        string_stream << file.rdbuf();
        BOOST_CHECK_EQUAL(string_stream.str(),"bulk: cmd1");
    }

////////////////////////////////////////////////////////////////////////////////////////////////

    BOOST_AUTO_TEST_CASE(current_time)
    {
        auto time = std::make_shared<std::time_t>();
        FileWriter writer(time);
        auto commands = std::make_shared<Commands>(Commands{"cmd1"});
        writer.update(commands);
        writer.print();
        BOOST_CHECK_EQUAL(std::time(nullptr),*time);
    }

////////////////////////////////////////////////////////////////////////////////////////////////

    BOOST_AUTO_TEST_CASE(delete_commands_console)
    {
        std::stringbuf out_buffer;
        std::ostream out_stream(&out_buffer);
        ConsoleWriter writer(out_stream);
        {
            auto commands = std::make_shared<Commands>(Commands{"cmd1", "cmd2"});
            writer.update(commands);
        }
        BOOST_CHECK_THROW(writer.print(),std::exception);
    }

////////////////////////////////////////////////////////////////////////////////////////////////

    BOOST_AUTO_TEST_CASE(delete_commands_file)
    {
        auto time = std::make_shared<std::time_t>();
        FileWriter writer(time);
        std::shared_ptr<Commands> commands;
        BOOST_CHECK_THROW(writer.update(commands),std::exception);
        BOOST_CHECK_THROW(writer.print(),std::exception);
    }

BOOST_AUTO_TEST_SUITE_END()