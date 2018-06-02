#include <boost/test/unit_test.hpp>
#include <boost/mpl/assert.hpp>

#include "../src/Handler.h"
#include "../src/Writers.h"
#include "helper.h"

using Commands = std::vector<std::string>;

BOOST_AUTO_TEST_SUITE(test_counter)

    BOOST_AUTO_TEST_CASE(simple_counter)
    {
        class SimpleWorker : public Statistics {
        public:
          std::weak_ptr<Counter> _counter;
          void addLine() {
              Statistics::addLine();
          }
          void addBlock() {
              Statistics::addBlock();
          }
          void addCommands(const uint& count) {
              Statistics::addCommands(count);
          }
        } worker;

        auto counter = std::make_shared<Counter>("simple");
        worker.setCounter(counter);
        worker.addBlock();
        worker.addCommands(3);

        std::stringbuf out_buffer1;
        std::ostream out_stream(&out_buffer1);
        out_stream << *counter;
        BOOST_CHECK_EQUAL(out_buffer1.str(),"simple поток - 3, 1");

        worker.addLine();
        worker.addLine();
        worker.addLine();
        worker.addLine();
        worker.addLine();
        worker.addLine();
        worker.addLine();
        worker.addLine();
        worker.addLine();
        worker.addLine();

        worker.addBlock();
        worker.addCommands(5);

        std::stringbuf out_buffer2;
        out_stream.rdbuf(&out_buffer2);
        out_stream << *counter;
        BOOST_CHECK_EQUAL(out_buffer2.str(),"simple поток - 10, 8, 2");
    }

////////////////////////////////////////////////////////////////////////////////////////////////

    BOOST_AUTO_TEST_CASE(writers_counter)
    {
        std::stringbuf out_buffer;
        std::ostream out_stream(&out_buffer);

        auto console_counter = std::make_shared<Counter>("console");
        auto file_counter = std::make_shared<Counter>("file");
        ConsoleWriter console_writer(out_stream);
        FileWriter file_writer;
        console_writer.setCounter(console_counter);
        file_writer.setCounter(file_counter);

        auto commands = std::make_shared<Commands>(Commands{"cmd1", "cmd2"});
        console_writer.update(commands);
        file_writer.update(commands);
        console_writer.print();
        file_writer.print();

        std::stringbuf out_console_buffer;
        out_stream.rdbuf(&out_console_buffer);
        out_stream << *console_counter;
        
        BOOST_CHECK_EQUAL(out_console_buffer.str(),"console поток - 2, 1");

        std::stringbuf out_file_buffer;
        out_stream.rdbuf(&out_file_buffer);
        out_stream << *file_counter;

        BOOST_CHECK_EQUAL(out_file_buffer.str(),"file поток - 2, 1");
    }

////////////////////////////////////////////////////////////////////////////////////////////////

    BOOST_AUTO_TEST_CASE(many_blocks)
    {
        std::stringbuf out_buffer;
        std::ostream out_stream(&out_buffer);

        auto console_counter = std::make_shared<Counter>("console");
        ConsoleWriter console_writer(out_stream);
        console_writer.setCounter(console_counter);

        auto commands = std::make_shared<Commands>(Commands{"cmd1", "cmd2"});
        console_writer.update(commands);
        console_writer.print();

        *commands = {"cmd3", "cmd4", "cmd5"};
        console_writer.print();

        std::stringbuf out_console_buffer;
        out_stream.rdbuf(&out_console_buffer);
        out_stream << *console_counter;
        
        BOOST_CHECK_EQUAL(out_console_buffer.str(),"console поток - 5, 2");
    }

////////////////////////////////////////////////////////////////////////////////////////////////

    BOOST_AUTO_TEST_CASE(handle_counter)
    {
        auto counter = std::make_shared<Counter>("main");
        Handler handler;
        handler.setCounter(counter);

        handler.setN(5);
        handler.addCommand("{");
        handler.addCommand("cmd1");
        handler.addCommand("cmd2");
        handler.addCommand("{");
        handler.addCommand("cmd3");
        handler.addCommand("cmd4");
        handler.addCommand("}");
        handler.addCommand("cmd5");
        handler.addCommand("cmd6");
        handler.addCommand("}");

        std::stringbuf out_buffer;
        std::ostream out_stream(&out_buffer);
        out_stream << *counter;
        
        BOOST_CHECK_EQUAL(out_buffer.str(),"main поток - 10, 6, 1");
    }

BOOST_AUTO_TEST_SUITE_END()