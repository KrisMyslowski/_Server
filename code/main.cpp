// QT HEADR
#include <QtWidgets/QApplication>
#include <QStringList>
#include <QFileInfo>

// _SERVER HEADER
#include "tcp_server.h"
#include "configview.h"
#include "datamodel.h"

namespace CONSTANTS
{
    namespace UI
    {
        static const QString WINDOW_TITLE = "Config View";
    }
        namespace ERROR
    {
        static const QString PATH_REQUIRED  = "Error: -c <path> is required. Use -h for help.";
        static const QString PATH_NOT_FOUND = "Error: Config file does not exist at path:";
    }
    namespace CLI
    {
        namespace PARAMS 
        {
            static const QString CONFIG_S   = "-c";
            //static const QString CONFIG_L = "-config";
            static const QString PORT_S     = "-p"; 
            //static const QString PORT_L   = "-port";
            static const QString HELP_S     = "-h";
            static const QString HELP_L     = "--help";
            static const QString UI         = "-ui";
        }
        void printHelp()
        {
            QTextStream out(stdout);
            out << "\nUsage: ./_Server -c <path> [-p <number>] [-ui]\n";
            out << "\nParameter:\n";
            out << "  -c <path>        REQ: Path to the JSON config file\n";                
            out << "  -p <number>      OPT: TCP port to listen on (default: 12345)\n";
            out << "  -ui              OPT: Start with config view\n";
            out << "  -h, --help       INF: Show this help message\n";
            out.flush();
        }
    }
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QStringList args = QCoreApplication::arguments();

    // - HELP
    if (args.contains(CONSTANTS::CLI::PARAMS::HELP_S) || 
        args.contains(CONSTANTS::CLI::PARAMS::HELP_L)) 
    {
        CONSTANTS::CLI::printHelp();
        return 0;
    }

    // - CONFIG
    int configIndex = args.indexOf(CONSTANTS::CLI::PARAMS::CONFIG_S);
    if (configIndex == -1 || configIndex + 1 >= args.size())
    {
        qCritical() << CONSTANTS::ERROR::PATH_REQUIRED;
        return 1;
    }

    // <CONFIGPATH>
    QString configPath = args.at(configIndex + 1);
    if (!QFileInfo::exists(configPath)) 
    {
        qCritical() << CONSTANTS::ERROR::PATH_NOT_FOUND << configPath;
        return 1;
    }

    // - PORT 
    quint16 port = 0;
    int portIndex = args.indexOf(CONSTANTS::CLI::PARAMS::PORT_S);
    if (portIndex != -1 && portIndex + 1 < args.size()) 
    {
        bool ok = false;
        int parsedPort = args.at(portIndex + 1).toInt(&ok);
        if (ok) 
        {
            port = static_cast<quint16>(parsedPort);
        }
    }

    // DATA MODEL: for JSON - Communication
    DataModel *dataModel = DataModel::getInstance(configPath);

    // MAIN APP: (simple solution for demo: manage request/respond, parsing, matching etc.)
    TCP_Server *tcp_server = new TCP_Server(dataModel, port, nullptr);

    // - UI: just display data and SET-updates. 
    ConfigView *window = nullptr;
    bool useUI = args.contains(CONSTANTS::CLI::PARAMS::UI);
    if (useUI) 
    {
        window = new ConfigView(dataModel, nullptr);
        window->show();

        // UI CONNECTIONS
        QObject::connect(tcp_server, &TCP_Server::clientConnected, window, &ConfigView::onClientConnected);
    }

    int ret = app.exec();

    if (window)
    {
        delete window;
    }
    delete tcp_server;
    return ret;
}