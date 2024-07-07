#include "CTCtrlEpoll.h"

CEpoll::CEpoll(int size)
	:m_epfd(-1)
{
	if (size >= 0) {
		m_epfd = epoll_create(size);
		if (m_epfd < 0) {
			m_epfd = -1;
		}
	}
}

CEpoll::~CEpoll()
{
	EClose();
}

int CEpoll::Init(int size)
{
	if (size < 0)return -1;
	if (m_epfd > 0)return 0;
	m_epfd = epoll_create(size);
	if (m_epfd < 0) {
		return -1;
	}
	return 0;
}

int CEpoll::Add_fd(int fd, epoll_event* ev)
{
	if (m_epfd < 0)return -1;
	if (fd < 0 || ev == NULL)return -1;
	return epoll_ctl(m_epfd, EPOLL_CTL_ADD, fd, ev);
}

int CEpoll::Mod_fd(int fd, epoll_event* ev)
{
	if (m_epfd < 0)return -1;
	if (fd < 0 || ev == NULL)return -1;
	return epoll_ctl(m_epfd, EPOLL_CTL_MOD, fd, ev);
}

int CEpoll::Del_fd(int fd)
{
	if (m_epfd < 0)return -1;
	if (fd < 0)return -1;
	return epoll_ctl(m_epfd, EPOLL_CTL_DEL, fd, NULL);
}

int CEpoll::EWait(epoll_event* evlist, int Maxevents, int timeout)
{
	if (m_epfd < 0 || Maxevents < 0)return -1;
	if (evlist == NULL)return -1;

	return epoll_wait(m_epfd, evlist, Maxevents, timeout);
}

int CEpoll::EClose()
{
	if (m_epfd > 0) {
		int temp = m_epfd;
		m_epfd = -1;
		close(temp);
	}
	return 0;
}
