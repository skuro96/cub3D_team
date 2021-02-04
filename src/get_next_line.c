/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hiwata <hiwata@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/06 00:20:53 by skurosu           #+#    #+#             */
/*   Updated: 2021/02/04 19:23:16 by hiwata           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"
#include "libft.h"

int		free_return(char **s1, char **s2, int ret)
{
	if (s1 && *s1)
	{
		free(*s1);
		*s1 = NULL;
	}
	if (s2 && *s2)
	{
		free(*s2);
		*s2 = NULL;
	}
	return (ret);
}

int		save_buf(char **save, char **buf)
{
	char *tmp;

	if (*save != NULL)
	{
		tmp = *save;
		if (!(*save = ft_strjoin(*save, *buf)))
			return (free_return(buf, &tmp, READ_ERROR));
		free(tmp);
	}
	else if (!(*save = ft_strndup(*buf, ft_strlen(*buf))))
		return (free_return(buf, NULL, READ_ERROR));
	return (0);
}

int		cut_endl(char **save, char **line)
{
	int		endl;
	char	*tmp;

	endl = ft_strchri(*save, '\n');
	if (endl < 0)
	{
		if (!(*line = ft_strndup(*save, ft_strlen(*save))))
			return (free_return(save, NULL, READ_ERROR));
		return (free_return(save, NULL, READ_EOF));
	}
	if (!(*line = ft_strndup(*save, endl)))
		return (free_return(save, NULL, READ_ERROR));
	tmp = *save;
	if (!(*save = ft_strndup(*save + endl + 1, ft_strlen(*save + endl + 1))))
		return (free_return(&tmp, line, READ_ERROR));
	return (free_return(&tmp, NULL, READ));
}

int		get_next_line(int fd, char **line)
{
	static char	*save;
	char		*buf;
	int			ret;

	if (!line || fd < 0 || BUFFER_SIZE <= 0)
		return (READ_ERROR);
	if (ft_strchri(save, '\n') != -1)
		return (cut_endl(&save, line));
	if (!(buf = malloc(sizeof(char) * (BUFFER_SIZE + 1))))
		return (free_return(&save, NULL, READ_ERROR));
	while ((ret = read(fd, buf, BUFFER_SIZE)) > 0)
	{
		buf[ret] = '\0';
		if (save_buf(&save, &buf) == READ_ERROR)
			return (READ_ERROR);
		if (ft_strchri(save, '\n') != -1)
			break ;
	}
	free(buf);
	if (ret < 0)
		return (free_return(&save, NULL, READ_ERROR));
	return (cut_endl(&save, line));
}
