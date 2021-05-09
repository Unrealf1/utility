/**
* JetBrains Space Automation
* This Kotlin-script file lets you automate build activities
* For more info, see https://www.jetbrains.com/help/space/automation.html
*/

job("Build and test") {
    parallel { // sequential {
    	container(displayName="g++ asan build", image = "unrealracoon/communicator-cpp-ci:0.0.2") {
            shellScript {
                interpreter = "/bin/sh"
                content = """                    
                    echo "compiling with g++"
                    ./testBuild.sh -DCMAKE_C_COMPILER=/usr/bin/gcc -DCMAKE_CXX_COMPILER=/usr/bin/g++
                    echo "testing with g++"
                    ./build/bin/Test
                """
            }
        }
        
        // container(displayName="g++ tsan build", image = "unrealracoon/communicator-cpp-ci:0.0.2") {
        //     shellScript {
        //         interpreter = "/bin/sh"
        //         content = """                    
        //             echo "compiling with g++"
        //             ./build.sh -DCMAKE_BUILD_TYPE=Debug -DENABLE_SANITIZER_THREAD=TRUE -DTESTS=TRUE -DCMAKE_C_COMPILER=/usr/bin/gcc -DCMAKE_CXX_COMPILER=/usr/bin/g++
        //             echo "testing with g++"
        //             ./build/bin/Test
        //         """
        //     }
        // }
        
         container(displayName="g++ release build", image = "unrealracoon/communicator-cpp-ci:0.0.2") {
            shellScript {
                interpreter = "/bin/sh"
                content = """                    
                    echo "compiling with g++"
                    ./build.sh -DCMAKE_BUILD_TYPE=Release -DTESTS=TRUE -DCMAKE_C_COMPILER=/usr/bin/gcc -DCMAKE_CXX_COMPILER=/usr/bin/g++
                    echo "testing with g++"
                    ./build/bin/Test
                """
            }
        }
        
        container(displayName="clang++ asan build", image = "unrealracoon/communicator-cpp-ci:0.0.2") {
            shellScript {
                interpreter = "/bin/sh"
                content = """                    
                    echo "compiling with clang++"
                    ./testBuild.sh -DCMAKE_C_COMPILER=/usr/bin/clang -DCMAKE_CXX_COMPILER=/usr/bin/clang++
                    echo "testing with clang++"
                    ./build/bin/Test
                """
            }
        }
        
        // container(displayName="clang++ tsan build", image = "unrealracoon/communicator-cpp-ci:0.0.2") {
        //     shellScript {
        //         interpreter = "/bin/sh"
        //         content = """                    
        //             echo "compiling with g++"
        //             ./build.sh -DCMAKE_BUILD_TYPE=Debug -DENABLE_SANITIZER_THREAD=TRUE -DTESTS=TRUE -DCMAKE_C_COMPILER=/usr/bin/clang -DCMAKE_CXX_COMPILER=/usr/bin/clang++
        //             echo "testing with clang++"
        //             ./build/bin/Test
        //         """
        //     }
        // }
        
        container(displayName="clang++ release build", image = "unrealracoon/communicator-cpp-ci:0.0.2") {
            shellScript {
                interpreter = "/bin/sh"
                content = """                    
                    echo "compiling with clang++"
                    ./build.sh -DCMAKE_BUILD_TYPE=Release -DTESTS=TRUE -DCMAKE_C_COMPILER=/usr/bin/gcc -DCMAKE_CXX_COMPILER=/usr/bin/clang++
                    echo "testing with g++"
                    ./build/bin/Test
                """
            }
        }
    }//}
}
