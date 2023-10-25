#include <iostream>
#include "glm.hpp"

int main()
{
    std::cout << "Layer Engine Lives!\n";

    glm::vec3 var1(1,2,3);
    glm::vec3 var2(4,5,6);
    glm::vec3 var3 = var1 + var2;
    std::cout << "GLM test Main: " << var3.x << "\n";

    return EXIT_SUCCESS;
}