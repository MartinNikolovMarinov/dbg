#include <iostream>
#include <iomanip>
#include <cstring>
#include <vector>
#include <string_view>
#include <string>
#include <unordered_map>
#include <algorithm>

#include <assert.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/personality.h>
#include <sys/user.h>

#include <linenoise.h>

enum struct Reg : int {
    DEFAULT,

    RAX, RBX, RCX, RDX,
    RDI, RSI, RBP, RSP,
    R8,  R9,  R10, R11,
    R12, R13, R14, R15,
    RIP, RFLAGS,    CS,
    ORIG_RAX, FS_BASE,
    GS_BASE,
    FS, GS, SS, DS, ES,

    COUNT,
};

struct RegDesc {
    RegDesc() : r(Reg::DEFAULT), dwarf(0), name("") {}
    RegDesc(Reg r, int dwarf, std::string_view name) : r(r), dwarf(dwarf), name(name) {}

    Reg r;
    int dwarf;
    std::string name;
};

const RegDesc g_RegisterDescriptors[size_t(Reg::COUNT)] = {
    { Reg::DEFAULT, -100, "not set" },
    { Reg::R15, 15, "r15" },
    { Reg::R14, 14, "r14" },
    { Reg::R13, 13, "r13" },
    { Reg::R12, 12, "r12" },
    { Reg::RBP, 6, "rbp" },
    { Reg::RBX, 3, "rbx" },
    { Reg::R11, 11, "r11" },
    { Reg::R10, 10, "r10" },
    { Reg::R9, 9, "r9" },
    { Reg::R8, 8, "r8" },
    { Reg::RAX, 0, "rax" },
    { Reg::RCX, 2, "rcx" },
    { Reg::RDX, 1, "rdx" },
    { Reg::RSI, 4, "rsi" },
    { Reg::RDI, 5, "rdi" },
    { Reg::ORIG_RAX, -1, "orig_rax" },
    { Reg::RIP, -1, "rip" },
    { Reg::CS, 51, "cs" },
    { Reg::RFLAGS, 49, "eflags" },
    { Reg::RSP, 7, "rsp" },
    { Reg::SS, 52, "ss" },
    { Reg::FS_BASE, 58, "fs_base" },
    { Reg::GS_BASE, 59, "gs_base" },
    { Reg::DS, 53, "ds" },
    { Reg::ES, 50, "es" },
    { Reg::FS, 54, "fs" },
    { Reg::GS, 55, "gs" },
};

const RegDesc& GetRegDesc(Reg r) {
    assert(size_t(r) < size_t(Reg::COUNT));
    for (size_t i = 0; i < size_t(Reg::COUNT); i++) {
        if (g_RegisterDescriptors[i].r == r) {
            return g_RegisterDescriptors[i];
        }
    }
    return g_RegisterDescriptors[0];
}

bool GetRegisterValue(pid_t pid, Reg r, uint64_t& out) {
    user_regs_struct regs;
    if (ptrace(PTRACE_GETREGS, pid, nullptr, &regs) < 0) {
        std::cerr << "Failed to get register value: " << strerror(errno) << std::endl;
        return false;
    }

    const RegDesc& reg = GetRegDesc(r);
    switch (reg.r) {
        case Reg::R15:        out = regs.r15; return true;
        case Reg::R14:        out = regs.r14; return true;
        case Reg::R13:        out = regs.r13; return true;
        case Reg::R12:        out = regs.r12; return true;
        case Reg::RBP:        out = regs.rbp; return true;
        case Reg::RBX:        out = regs.rbx; return true;
        case Reg::R11:        out = regs.r11; return true;
        case Reg::R10:        out = regs.r10; return true;
        case Reg::R9:         out = regs.r9; return true;
        case Reg::R8:         out = regs.r8; return true;
        case Reg::RAX:        out = regs.rax; return true;
        case Reg::RCX:        out = regs.rcx; return true;
        case Reg::RDX:        out = regs.rdx; return true;
        case Reg::RSI:        out = regs.rsi; return true;
        case Reg::RDI:        out = regs.rdi; return true;
        case Reg::ORIG_RAX:   out = regs.orig_rax; return true;
        case Reg::RIP:        out = regs.rip; return true;
        case Reg::CS:         out = regs.cs; return true;
        case Reg::RFLAGS:     out = regs.eflags; return true;
        case Reg::RSP:        out = regs.rsp; return true;
        case Reg::SS:         out = regs.ss; return true;
        case Reg::FS_BASE:    out = regs.fs_base; return true;
        case Reg::GS_BASE:    out = regs.gs_base; return true;
        case Reg::DS:         out = regs.ds; return true;
        case Reg::ES:         out = regs.es; return true;
        case Reg::FS:         out = regs.fs; return true;
        case Reg::GS:         out = regs.gs; return true;

        case Reg::DEFAULT:  return false;
        default:            return false;
    }

    return false;
}

bool SetRegisterValue(pid_t pid, Reg r, uint64_t value) {
    user_regs_struct regs;
    ptrace(PTRACE_GETREGS, pid, nullptr, &regs);

    const RegDesc& reg = GetRegDesc(r);
    switch (reg.r) {
        case Reg::R15:         regs.r15 = value; break;
        case Reg::R14:         regs.r14 = value; break;
        case Reg::R13:         regs.r13 = value; break;
        case Reg::R12:         regs.r12 = value; break;
        case Reg::RBP:         regs.rbp = value; break;
        case Reg::RBX:         regs.rbx = value; break;
        case Reg::R11:         regs.r11 = value; break;
        case Reg::R10:         regs.r10 = value; break;
        case Reg::R9:          regs.r9 = value; break;
        case Reg::R8:          regs.r8 = value; break;
        case Reg::RAX:         regs.rax = value; break;
        case Reg::RCX:         regs.rcx = value; break;
        case Reg::RDX:         regs.rdx = value; break;
        case Reg::RSI:         regs.rsi = value; break;
        case Reg::RDI:         regs.rdi = value; break;
        case Reg::ORIG_RAX:    regs.orig_rax = value; break;
        case Reg::RIP:         regs.rip = value; break;
        case Reg::CS:          regs.cs = value; break;
        case Reg::RFLAGS:      regs.eflags = value; break;
        case Reg::RSP:         regs.rsp = value; break;
        case Reg::SS:          regs.ss = value; break;
        case Reg::FS_BASE:     regs.fs_base = value; break;
        case Reg::GS_BASE:     regs.gs_base = value; break;
        case Reg::DS:          regs.ds = value; break;
        case Reg::ES:          regs.es = value; break;
        case Reg::FS:          regs.fs = value; break;
        case Reg::GS:          regs.gs = value; break;

        case Reg::DEFAULT:     return false;
        default:               return false;
    }

    if (int err = ptrace(PTRACE_SETREGS, pid, nullptr, &regs); err < 0) {
        std::cerr << "Failed to set register: " << std::strerror(errno) << std::endl;
        return -3;
    }

    return true;
}

bool GetRegisterValueFromDwarfRegister(pid_t pid, int regnum, uint64_t& out) {
    const RegDesc* reg;
    for (size_t i = 0; i < size_t(Reg::COUNT); i++) {
        if (g_RegisterDescriptors[i].dwarf == regnum) {
            reg = &g_RegisterDescriptors[i];
        }
    }
    if (reg == nullptr) {
        return false;
    }

    bool ret = GetRegisterValue(pid, reg->r, out);
    return ret;
}

std::string GetRegisterName(Reg r) { return GetRegDesc(r).name; }

Reg GetRegisterFromName(std::string_view name) {
    for (size_t i = 0; i < size_t(Reg::COUNT); i++) {
        if (g_RegisterDescriptors[i].name == name) {
            return g_RegisterDescriptors[i].r;
        }
    }
    return g_RegisterDescriptors[0].r;
}

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

void LogArguments(std::vector<std::string_view> args) {
    for (size_t i = 0; i < args.size(); i++) {
        std::cout << "args[" << i << "] = " << args[i] << std::endl;
    }
}

struct Breakpoint {
public:
    Breakpoint() : m_pid(0), m_addr(0), m_enabled(false), m_savedData(0) {}
    Breakpoint(pid_t pid, intptr_t addr)
        : m_pid(pid), m_addr(addr), m_enabled(false), m_savedData(0) {}

    bool IsEnabled() const { return m_enabled; }
    uint8_t GetAddr() const { return m_addr; }

    void Enable() {
        long data = ptrace(PTRACE_PEEKDATA, m_pid, m_addr, nullptr);
        m_savedData = static_cast<uint8_t>(data & 0xFF); // save last byte
        uint64_t int3 = 0xCC;
        uint64_t data_with_int3 = (data & ~0xFF) | int3; // set last byte to int3
        ptrace(PTRACE_POKEDATA, m_pid, m_addr, data_with_int3);

        m_enabled = true;
    }

    void Disable() {
        long data = ptrace(PTRACE_PEEKDATA, m_pid, m_addr, nullptr);
        long restored_data = (data & ~0xFF) | m_savedData;
        ptrace(PTRACE_POKEDATA, m_pid, m_addr, restored_data);

        m_enabled = false;
    }

private:
    pid_t m_pid;
    intptr_t m_addr;
    bool m_enabled;
    uint8_t m_savedData;
};

struct Debugger {

    Debugger(std::string_view progName, pid_t pid)
        : m_progName(progName), m_pid(pid) {}

    int Run() {
        if (int ret = WaitForSignal(); ret < 0) {
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

    void SetBreakpointAtAddress(intptr_t addr) {
        Breakpoint bp(m_pid, addr);
        bp.Enable();
        m_breakpoints[addr] = bp;
    }

    void DumpRegisters() {
        for (const auto& rd : g_RegisterDescriptors) {
            uint64_t regVal;
            if (!GetRegisterValue(m_pid, rd.r, regVal)) {
                std::cout << "error getting register " << rd.name << std::endl;
                continue;
            }
            std::cout << rd.name << " 0x" << std::setfill('0') << std::setw(16) << std::hex << regVal << std::endl;
        }
    }

    uint64_t ReadMemory(uint64_t address) {
        // mTODO: use https://man7.org/linux/man-pages/man2/process_vm_readv.2.html to bypass kernel space calls
        return ptrace(PTRACE_PEEKDATA, m_pid, address, nullptr);
    }

    int WriteMemory(uint64_t address, uint64_t value) {
        // mTODO: use https://man7.org/linux/man-pages/man2/process_vm_readv.2.html to bypass kernel space calls
        return ptrace(PTRACE_POKEDATA, m_pid, address, value);
    }

    uint64_t GetPC() {
        uint64_t ret;
        if (!GetRegisterValue(m_pid, Reg::RIP, ret)) {
            std::cout << "error getting PC" << std::endl;
            return 0;
        }
        return ret;
    }

    bool SetPC(uint64_t pc) {
        if (!SetRegisterValue(m_pid, Reg::RIP, pc)) {
            std::cout << "error setting PC" << std::endl;
            return false;
        }
        return true;
    }

    bool StepOverBreakpoint() {
        uint64_t possibleBPLocation = GetPC() - 1;

        if (m_breakpoints.count(possibleBPLocation)) {
            auto& bp = m_breakpoints[possibleBPLocation];
            if (bp.IsEnabled()) {
                auto prevInstrAddr = possibleBPLocation;
                SetPC(prevInstrAddr);

                bp.Disable();
                ptrace(PTRACE_SINGLESTEP, m_pid, nullptr, nullptr);
                WaitForSignal();
                bp.Enable();
            }
        }

        return true;
    }

    int ContinueExecution() {
        if (!StepOverBreakpoint()) return -1;
        ptrace(PTRACE_CONT, m_pid, nullptr, nullptr);
        return WaitForSignal();
    }

private:
    int HandleCmd(std::string_view line) {
        constexpr std::string_view COMMAND_SEPARATOR = " ";
        auto args = Split(line, COMMAND_SEPARATOR);
        if (args.empty()) return 0;
        std::string_view command = args[0];
        LogArguments(args);

        if (HasPrefix(command, "cont")) {
            return ContinueExecution();
        }
        else if (HasPrefix(command, "break") && args.size() == 2) {
            std::string addrStr (args[1].substr(2));
            intptr_t addr = static_cast<intptr_t>(std::stol(addrStr, 0, 16));
            SetBreakpointAtAddress(addr);
        }
        else if (HasPrefix(command, "register") && args.size() >= 2) {
            if (HasPrefix(args[1], "dump" ) && args.size() == 2) {
                DumpRegisters();
            }
            else if (HasPrefix(args[1], "read") && args.size() == 3) {
                uint64_t regVal;
                if (!GetRegisterValue(m_pid, GetRegisterFromName(args[2]), regVal)) {
                    std::cout << "error getting register " << args[2] << std::endl;
                    return -1;
                }
                std::cout << std::hex << "0x" << regVal << std::endl;
            }
            else if (HasPrefix(args[1], "write") && args.size() == 4) {
                std::string val {args[3].substr(2)};
                SetRegisterValue(m_pid, GetRegisterFromName(args[2]), std::stol(val, 0, 16));
            }
        }
        else if (HasPrefix(command, "memory")) {
            std::string addr {args[2].substr(2)};
            if (HasPrefix(args[1], "read")) {
                std::cout << std::hex << "0x" << ReadMemory(std::stol(addr, 0, 16)) << std::endl;
            }
            else if (HasPrefix(args[1], "write")) {
                std::string val {args[3].substr(2)};
                if (WriteMemory(std::stol(addr, 0, 16), std::stol(val, 0, 16)) <= 0) {
                    std::cerr << "error writing memory: " << strerror(errno) << std::endl;
                    return -1;
                }
            }
        }
        else {
            std::cerr << "Unknown command\n";
        }

        return 0;
    }

    int WaitForSignal() {
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
    std::unordered_map<uintptr_t, Breakpoint> m_breakpoints;
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
        personality(ADDR_NO_RANDOMIZE); // TODO: temporary disable address layout randomization for the debugged process.
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
