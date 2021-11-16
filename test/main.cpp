#include <iostream>

#include <errorsystem/status.h>

using namespace std;

int main()
{
    cout << "Hello World!" << endl;
    errorsystem::Status s(1, "hello world");
    if (!s.ok()) {
        std::cout << "error, code: " << s.getCode() << ", msg: " << s.getMessage() << std::endl;
    }
    return 0;
}
