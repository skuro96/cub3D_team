/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hiwata <hiwata@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/06 00:21:06 by skurosu           #+#    #+#             */
/*   Updated: 2021/02/04 19:24:25 by hiwata           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GET_NEXT_LINE_H
# define GET_NEXT_LINE_H

# include <stdio.h>
# include <unistd.h>
# include <stdlib.h>
# include <fcntl.h>

# define READ 1
# define READ_EOF 0
# define READ_ERROR -1

# define BUFFER_SIZE 64

char	*ft_strndup(char const *str, size_t len);
int		ft_strchri(char *str, char c);
int		free_return(char **s1, char **s2, int ret);
int		save_buf(char **save, char **buf);
int		cut_endl(char **save, char **line);
int		get_next_line(int fd, char **line);

#endif
