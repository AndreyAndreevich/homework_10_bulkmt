#include <boost/test/unit_test.hpp>
#include <boost/mpl/assert.hpp>

#include "Handler.h"
#include "Writers.h"

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

        int lines,commands,blocks;
        std::tie(lines,commands,blocks) = counter->get();
        BOOST_CHECK_EQUAL(lines,0);
        BOOST_CHECK_EQUAL(commands,3);
        BOOST_CHECK_EQUAL(blocks,1);

        std::stringbuf out_buffer1;
        std::ostream out_stream(&out_buffer1);
        out_stream << *counter;
        BOOST_CHECK_EQUAL(out_buffer1.str(),"simple поток - комманд: 3, блоков: 1");

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

        std::tie(lines,commands,blocks) = counter->get();
        BOOST_CHECK_EQUAL(lines,10);
        BOOST_CHECK_EQUAL(commands,8);
        BOOST_CHECK_EQUAL(blocks,2);

        std::stringbuf out_buffer2;
        out_stream.rdbuf(&out_buffer2);
        out_stream << *counter;
        BOOST_CHECK_EQUAL(out_buffer2.str(),"simple поток - строк: 10, комманд: 8, блоков: 2");
    }

////////////////////////////////////////////////////////////////////////////////////////////////

    BOOST_AUTO_TEST_CASE(writers_counter)
    {
        std::stringstream out_stream;

        auto console_counter = std::make_shared<Counter>("console");
        auto file_counter = std::make_shared<Counter>("file");
        ConsoleWriter console_writer(out_stream);
        FileWriter file_writer;
        console_writer.setCounter(console_counter);
        file_writer.setCounter(file_counter);

        auto commands = std::make_shared<Commands>(Commands{"cmd1"});
        console_writer.update(commands);
        file_writer.update(commands);
        commands->push_back(std::string{"cmd2"});
        console_writer.update(commands);
        file_writer.update(commands);
        console_writer.print();
        file_writer.print();
        
        std::remove(file_writer.getName().c_str());

        int lines_c,commands_c,blocks_c;
        std::tie(lines_c,commands_c,blocks_c) = console_counter->get();
        BOOST_CHECK_EQUAL(lines_c,0);
        BOOST_CHECK_EQUAL(commands_c,2);
        BOOST_CHECK_EQUAL(blocks_c,1);
        
        std::tie(lines_c,commands_c,blocks_c) = file_counter->get();
        BOOST_CHECK_EQUAL(lines_c,0);
        BOOST_CHECK_EQUAL(commands_c,2);
        BOOST_CHECK_EQUAL(blocks_c,1);
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

        int lines_c,commands_c,blocks_c;
        std::tie(lines_c,commands_c,blocks_c) = console_counter->get();
        BOOST_CHECK_EQUAL(lines_c,0);
        BOOST_CHECK_EQUAL(commands_c,5);
        BOOST_CHECK_EQUAL(blocks_c,2);
    }

////////////////////////////////////////////////////////////////////////////////////////////////

    BOOST_AUTO_TEST_CASE(handle_counter)
    {
        auto counter = std::make_shared<Counter>("main");
        Handler handler(5);
        handler.setCounter(counter);

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

        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        int lines_c,commands_c,blocks_c;
        std::tie(lines_c,commands_c,blocks_c) = counter->get();
        BOOST_CHECK_EQUAL(lines_c,10);
        BOOST_CHECK_EQUAL(commands_c,6);
        BOOST_CHECK_EQUAL(blocks_c,1);

        std::stringbuf out_buffer;
        std::ostream out_stream(&out_buffer);
        out_stream << *counter;
        
        BOOST_CHECK_EQUAL(out_buffer.str(),"main поток - строк: 10, комманд: 6, блоков: 1");
    }

BOOST_AUTO_TEST_SUITE_END()
