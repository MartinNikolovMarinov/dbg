#include <iostream>
#include <cstring>
#include <vector>
#include <string_view>
#include <string>

#include <unistd.h>
#include <errno.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <linenoise.h>

inline std::vector<std::string_view> Split(std::string_view str, std::string_view delim) {
    std::vector<std::string_view> tokens;
    size_t prev = 0, pos = 0;
    do {
        pos = str.find(delim, prev);
        if (pos == std::string_view::npos) pos = str.length();
        std::string_view token = str.substr(prev, pos-prev);
        if (!token.empty()) tokens.push_back(token);
        prev = pos + delim.length();
    }
    while (pos < str.length() && prev < str.length());
    return tokens;
}

bool HasPrefix(std::string_view s, std::string_view prefix) {
    if (s.size() > prefix.size()) return false;
    return std::equal(s.begin(), s.end(), prefix.begin());
}

struct Debugger {

    Debugger(std::string_view progName, pid_t pid)
        : m_progName(progName), m_pid(pid) {}

    int Run() {
        if (int ret = WaitPid(); ret < 0) {
            return ret;
        }

        char* line = nullptr;
        while ((line = linenoise("dbg> ")) != nullptr) {
            std::cout << "line: " << line << std::endl;
            if(int err = HandleCmd(line); err < 0) {
                return err;
            }
            linenoiseHistoryAdd(line);
            linenoiseFree(line);
        }

        return 0;
    }

private:
    int HandleCmd(std::string_view line) {
        auto args = Split(line, " ");
        std::string_view command = args[0];

        if (HasPrefix(command, "cont")) {
            if (int ret = ContinueExecution(); ret < 0) {
                return ret;
            }
        }
        else {
            std::cerr << "Unknown command\n";
        }

        return 0;
    }

    int ContinueExecution() {
        ptrace(PTRACE_CONT, m_pid, nullptr, nullptr);
        return WaitPid();
    }

    int WaitPid() {
        int waitStatus;
        int options = 0;
        pid_t pid = waitpid(m_pid, &waitStatus, options);
        if (pid < 0) {
            std::cerr << "waitpid failed: " << strerror(errno) << std::endl;
            return -5;
        }
        return 0;
    }

    std::string m_progName;
    pid_t m_pid;
};

int ExecDebuggedProgram(std::string_view progName) {
    if (int err = ptrace(PTRACE_TRACEME, 0, nullptr, nullptr); err < 0) {
        std::cerr << "Failed to trace process: " << std::strerror(errno) << std::endl;
        return -3;
    }
    if (int err = execl(progName.data(), progName.data(), nullptr); err < 0) {
        std::cerr << "Failed to exec program: " << std::strerror(errno) << std::endl;
        return -4;
    }
    return 0;
}

void pprint (std::string_view str) {
    std::cout << str << std::endl;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "No progam to debug" << std::endl;
        return -1;
    }

    char* prog = argv[1];
    pid_t pid = fork();
    if (pid < 0) {
        std::cerr << "Failed to fork reason: " << std::strerror(errno) << std::endl;
        return -2;
    }

    if (pid == 0) {
        std::cout << "Execute Debugged Program" << '\n';
        return ExecDebuggedProgram(prog);
    }
    else {
        std::cout << "Starting Debugging Process" << '\n';
        Debugger dbg (prog, pid);
        if (int ret = dbg.Run(); ret < 0) {
            return ret;
        }
    }

    return 0;
}
