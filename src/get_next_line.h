/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skurosu <skurosu@student.42tokyo.j>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/06 00:21:06 by skurosu           #+#    #+#             */
/*   Updated: 2020/11/06 00:22:33 by skurosu          ###   ########.fr       */
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

size_t	ft_strlen(char *str);
char	*ft_substr(char const *str, size_t len);
char	*ft_strjoin(char *s1, char *s2);
int		ft_strchr(char *str, char c);
int		free_return(char **s1, char **s2, int ret);
int		save_buf(char **save, char **buf);
int		cut_endl(char **save, char **line);
int		get_next_line(int fd, char **line);

#endif
