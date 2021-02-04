#include"libft.h"


static size_t	count_strs(const char *str, char c)
{
	size_t	i;
	size_t	cnt;
	int		flag;

	i = 0;
	cnt = 0;
	flag = 0;
	while (str[i] != '\0')
	{
		if (str[i] == c)
			flag = 0;
		else if (flag == 0)
		{
			cnt++;
			flag = 1;
		}
		i++;
	}
	return (cnt);
}

static void		my_strcpy(char *dest, const char *src, size_t len)
{
	size_t i;

	i = 0;
	while (i < len - 1)
	{
		dest[i] = src[i];
		i++;
	}
	dest[i] = '\0';
}

static char		**all_free(char **dest, int n)
{
	int i;

	i = 0;
	while (i < n)
	{
		free(dest[i]);
		i++;
	}
	free(dest);
	return (NULL);
}

char			**ft_split(char const *s, char c)
{
	size_t	i;
	size_t	len;
	size_t	str_nbr;
	char	**dest;

	if (!s || !(dest = malloc(sizeof(char*) * (count_strs(s, c) + 1))))
		return (NULL);
	i = 0;
	str_nbr = 0;
	while (s[i] != '\0' && str_nbr < count_strs(s, c))
	{
		while (s[i] != '\0' && s[i] == c)
			i++;
		len = 0;
		while (s[i + len] != '\0' && s[i + len] != c)
			len++;
		if (!(dest[str_nbr] = malloc(sizeof(char) * (len + 1))))
			return (all_free(dest, str_nbr));
		my_strcpy(dest[str_nbr], &s[i], len + 1);
		str_nbr++;
		i += len;
	}
	dest[str_nbr] = (NULL);
	return (dest);
}
