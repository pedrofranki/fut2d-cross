#ifndef FCPORTUGAL_SETPLAY_EXCEPTION_H
#define FCPORTUGAL_SETPLAY_EXCEPTION_H

class FCPortugalSetplayException: public std::exception {
private:
    std::string message_;
public:
    FCPortugalSetplayException(const std::string& message): message_(message) {};
    virtual ~FCPortugalSetplayException() throw (){}
    virtual const char* what() const throw() {
        return message_.c_str();
    }
};
#endif

