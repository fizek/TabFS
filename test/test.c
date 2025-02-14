#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>

#include <unistd.h>
#include <assert.h>
#include <wordexp.h>
#include <regex.h>

int file_contents_equal(char* path, char* contents) {
    // hehe: https://twitter.com/ianh_/status/1340450349065244675
    setenv("path", path, 1);
    setenv("contents", contents, 1);
    return system("[ \"$contents\" == \"$(cat \"$path\")\" ]") == 0;
}

char* expand(char* phrase) { // expand path with wildcard
    wordexp_t result; assert(wordexp(phrase, &result, 0) == 0);
    return result.we_wordv[0];
}

int matches_regex(char* str, char* pattern) {
    regex_t re; assert(regcomp(&re, pattern, REG_EXTENDED) == 0);
    int i = regexec(&re, str, 0, NULL, 0);
    regfree(&re);
    return i == 0;
}

// integration tests
int main() {
    // TODO: invoke over extension
    /* assert(system("node ../extension/background.js --unhandled-rejections=strict") == 0); // run quick local JS tests */

    // reload the extension so we know it's the latest code.
    system("echo reload > ../fs/mnt/runtime/reload 2>/dev/null"); // this may error, but it should still have effect
    // spin until the extension reloads.
    struct stat st; while (stat("../fs/mnt/tabs", &st) != 0) {}

    assert(file_contents_equal(expand("../fs/mnt/extensions/TabFS*/enabled"), "true"));

    {
        assert(system("echo about:blank > ../fs/mnt/tabs/create") == 0);
        int times = 0;
        for (;;) {
            if (file_contents_equal("../fs/mnt/tabs/last-focused/url.txt", "about:blank")) {
                break;
            }
            usleep(10000);
            assert(times++ < 10000);
        }

        assert(system("echo remove > ../fs/mnt/tabs/last-focused/control") == 0);
    }

    {
        assert(system("echo file://$(pwd)/test-page.html > ../fs/mnt/tabs/create") == 0);
        assert(file_contents_equal("../fs/mnt/tabs/last-focused/title.txt", "Title of Test Page"));
        assert(file_contents_equal("../fs/mnt/tabs/last-focused/text.txt", "Body Text of Test Page"));

        assert(system("ls ../fs/mnt/tabs/last-focused/debugger/scripts") == 0);

        {
            DIR* scripts = opendir("../fs/mnt/tabs/last-focused/debugger/scripts");
            assert(strcmp(readdir(scripts)->d_name, ".") == 0);
            assert(strcmp(readdir(scripts)->d_name, "..") == 0);
            assert(matches_regex(readdir(scripts)->d_name, "test\\-script.js$"));
            closedir(scripts);
        }
        assert(system("cat ../fs/mnt/tabs/last-focused/debugger/scripts/*test-script.js") == 0);

        {
            FILE* console = fopen("../fs/mnt/tabs/last-focused/console", "r");
            assert(system("echo \"console.log('hi')\" > ../fs/mnt/tabs/last-focused/execute-script") == 0);
            char hi[3] = {0}; fread(hi, 1, 2, console);
            assert(strcmp(hi, "hi") == 0);
            fclose(console);
        }
        
        assert(system("echo remove > ../fs/mnt/tabs/last-focused/control") == 0);
    }

    assert(1); printf("Done!\n"); 
}
