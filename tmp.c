#include "libft.h"

int main(void)
{
    char *line = "";

    char **buf = ft_split(line, ' ');

    if (!buf[0])
        printf("null");
    return (0);
}