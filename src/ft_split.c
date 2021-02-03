#include "libft.h"

int		ary_size(const char *str, char c)
{
	int		i;

	i = 0;
	while (*str)
	{
		while (*str == c)
			str++;
		if (*str == '\0')
			return (i);
		while (*str && *str != c)
			str++;
		i++;
	}
	return (i);
}

void	free_all(char **array, int n)
{
	int i;

	i = 0;
	while (i < n)
	{
		free(array[i]);
		i++;
	}
	free(array);
}

char	**into_array(char **array, char *str, char c)
{
	int		len;
	int		i;

	i = 0;
	while (array)
	{
		while (*str && *str == c)
			str++;
		if (!*str)
			break ;
		len = 0;
		while (str[len] && str[len] != c)
			len++;
		if (!(array[i] = (ft_substr(str, 0, len))))
		{
			free_all(array, i);
			return (NULL);
		}
		i++;
		str += len;
	}
	array[i] = NULL;
	return (array);
}

char	**ft_split(const char *str, char c)
{
	char	**array;

	if (!str)
		return (NULL);
	if (!(array = malloc(sizeof(char *) * (ary_size(str, c) + 1))))
		return (NULL);
	return (into_array(array, (char*)str, c));
}