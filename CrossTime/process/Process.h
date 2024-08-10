#pragma once

#include "CTCtrlFunction.h"
#include <memory.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <Process.h>
#include <unistd.h>

// ���̿�����
class CProcess
{
public:
	/**
	* @brief ���캯��
	*/
	CProcess() {
		m_func = NULL;
		memset(m_pipes, 0, sizeof(m_pipes));
	}
	/**
	* @brief ��������
	*/
	~CProcess() {
		if (m_func != NULL) {
			delete m_func;
			m_func = NULL;
		}
	}
	/**
	* @brief ���ý�����ں���
	* @param func  ��ں���ָ��
	* @param args  ��ں����Ĳ����������õ��ǿɱ����ģ�壬��Ϊÿ�����̲����ĸ����ǲ��̶���
	*
	* @return ����ֵΪ0�����ʾ������ں������óɹ���Ϊ��0�����ʾ��ں�������ʧ��
	*/
	template<typename _FUNCTION_, typename... _ARGS_>
	int SetEntryFunction(_FUNCTION_ func, _ARGS_... args) {
		m_func = new CFunction<_FUNCTION_, _ARGS_...>(func, args...);
		if (m_func == NULL) {
			return -1;
		}
		return 0;
	}
	/**
	* @brief �����ܵ����ڽ��̼�ͨ�ţ���������
	*
	* @return ����ֵΪ0����ʾ�����ɹ���Ϊ��0�����ʾ����ʧ��
	*/
	int CreateSubProcess() {
		if (m_func == NULL)
			return -1;
		int ret = socketpair(AF_LOCAL, SOCK_STREAM, 0, m_pipes);  // �����׽���
		printf("%s(%d):<%s> pipe[0]=%d, pipe[1]=%d\n", __FILE__, __LINE__, __FUNCTION__, m_pipes[0], m_pipes[1]);
		if (ret == -1)
			return -2;
		pid_t pid = fork();
		if (pid == -1)
			return -2;
		if (pid == 0) {
			// �ӽ���
			close(m_pipes[1]); // �رյ�д
			m_pipes[1] = 0;
			ret = (*m_func)();
			exit(0);
		}
		// ������
		close(m_pipes[0]);
		m_pipes[0] = 0;
		m_pid = pid;
		return 0;
	}
	/**
	* @brief ͨ���ܵ�����̷����ļ�����������
	* @param fd Ҫ���͵��ļ�������
	*
	* @return ����ֵΪ0�����ʾ���ͳɹ���Ϊ��0�����ʾ����ʧ��
	*/
	int SendFD(int fd) { // ������
		struct msghdr msg;
		memset(&msg, 0, sizeof(msg));
		iovec iov[2];
		char buf[2][10] = { "1111","2222" };
		iov[0].iov_base = buf[0];
		iov[0].iov_len = sizeof(buf[0]);
		iov[1].iov_base = buf[1];
		iov[1].iov_len = sizeof(buf[1]);
		msg.msg_iov = iov;
		msg.msg_iovlen = 2;
		// ��������ݣ�����������Ҫ���ݵ�
		cmsghdr* cmsg = (cmsghdr*)calloc(1, CMSG_LEN(sizeof(int)));
		if (cmsg == NULL)return -1;
		cmsg->cmsg_len = CMSG_LEN(sizeof(int));
		cmsg->cmsg_level = SOL_SOCKET; // �������׽��ֵ�����
		cmsg->cmsg_type = SCM_RIGHTS;
		*((int*)CMSG_DATA(cmsg)) = fd;
		msg.msg_control = cmsg;
		msg.msg_controllen = cmsg->cmsg_len;
		ssize_t ret = sendmsg(m_pipes[1], &msg, 0);
		free(cmsg);
		if (ret == -1) {
			printf("%s(%d):<%s> ret=%ld\n", __FILE__, __LINE__, __FUNCTION__, ret);
			return -2;
		}
		return 0;
	}
	/**
	* @brief ͨ���ܵ������ļ�����������
	* @param fd ������յ����ļ�������
	*
	* @return ����ֵΪ0�����ʾ���ճɹ���Ϊ��0�����ʾ����ʧ��
	*/
	int RecvFD(int& fd) {
		struct msghdr msg;
		memset(&msg, 0, sizeof(msg));
		iovec iov[2];
		char buf[][10] = { "","" };
		iov[0].iov_base = buf[0];
		iov[0].iov_len = sizeof(buf[0]);
		iov[1].iov_base = buf[1];
		iov[1].iov_len = sizeof(buf[1]);
		msg.msg_iov = iov;
		msg.msg_iovlen = 2;

		cmsghdr* cmsg = (cmsghdr*)calloc(1, CMSG_LEN(sizeof(int)));
		if (cmsg == NULL)return -1;
		cmsg->cmsg_len = CMSG_LEN(sizeof(int));
		cmsg->cmsg_level = SOL_SOCKET;
		cmsg->cmsg_type = SCM_RIGHTS;
		msg.msg_control = cmsg;
		msg.msg_controllen = CMSG_LEN(sizeof(int));
		ssize_t ret = recvmsg(m_pipes[0], &msg, 0);
		if (ret == -1) {
			free(cmsg);
			return -2;
		}
		fd = *(int*)CMSG_DATA(cmsg);
		free(cmsg);
		return 0;
	}
	/**
	* @brief ͨ���ܵ�����̷��������׽��ֺ͵�ַ��Ϣ
	* @param fd   Ҫ���͵��׽����ļ�������
	* @param addrin  Ҫ���͵��ļ���������ַ��Ϣ
	*
	* @return ����ֵΪ0�����ʾ���ͳɹ���Ϊ��0�����ʾ����ʧ��
	*/
	int SendSocket(int fd, const sockaddr_in* addrin) { // ������
		struct msghdr msg;
		memset(&msg, 0, sizeof(msg));
		iovec iov;
		char buf[20] = "";
		memcpy(buf, addrin, sizeof(sockaddr_in));
		iov.iov_base = buf;
		iov.iov_len = sizeof(buf);
		msg.msg_iov = &iov;
		msg.msg_iovlen = 1;
		// ��������ݣ�����������Ҫ���ݵ�
		cmsghdr* cmsg = (cmsghdr*)calloc(1, CMSG_LEN(sizeof(int)));
		if (cmsg == NULL)return -1;
		cmsg->cmsg_len = CMSG_LEN(sizeof(int));
		cmsg->cmsg_level = SOL_SOCKET; // �������׽��ֵ�����
		cmsg->cmsg_type = SCM_RIGHTS;
		*((int*)CMSG_DATA(cmsg)) = fd;
		msg.msg_control = cmsg;
		msg.msg_controllen = cmsg->cmsg_len;
		ssize_t ret = sendmsg(m_pipes[1], &msg, 0);
		free(cmsg);
		if (ret == -1) {
			printf("%s(%d):<%s> ret=%ld\n", __FILE__, __LINE__, __FUNCTION__, ret);
			return -2;
		}
		return 0;
	}
	/**
	* @brief ͨ���ܵ����������׽��ֺ͵�ַ��Ϣ
	* @param fd  ������յ����׽����ļ�������
	* @param addrin  ������յ����׽�����������ַ��Ϣ
	*
	* @return ����ֵΪ0�����ʾ���ճɹ���Ϊ��0�����ʾ����ʧ��
	*/
	int RecvSocket(int& fd, sockaddr_in* addrin) {
		struct msghdr msg;
		memset(&msg, 0, sizeof(msg));
		iovec iov;
		char buf[20] = "";
		iov.iov_base = buf;
		iov.iov_len = sizeof(buf);
		msg.msg_iov = &iov;
		msg.msg_iovlen = 1;

		cmsghdr* cmsg = (cmsghdr*)calloc(1, CMSG_LEN(sizeof(int)));
		if (cmsg == NULL)return -1;
		cmsg->cmsg_len = CMSG_LEN(sizeof(int));
		cmsg->cmsg_level = SOL_SOCKET;
		cmsg->cmsg_type = SCM_RIGHTS;
		msg.msg_control = cmsg;
		msg.msg_controllen = CMSG_LEN(sizeof(int));
		ssize_t ret = recvmsg(m_pipes[0], &msg, 0);
		if (ret == -1) {
			free(cmsg);
			return -2;
		}
		fd = *(int*)CMSG_DATA(cmsg);
		memcpy(addrin, buf, sizeof(sockaddr_in));
		free(cmsg);
		return 0;
	}
	/**
	* @brief �����ػ�����
	*
	* @return ����ֵΪ0�����ʾ�ػ����̴����ɹ���Ϊ��0�����ʾ�ػ����̴���ʧ��
	*/
	static int SwitchDeamon() {
		pid_t ret = fork();
		if (ret == -1)return -1;
		if (ret > 0) {
			// �����̵���Ϊֹ
			exit(0);
		}
		// �ӽ�����������
		ret = setsid();
		if (ret == -1)return -2;
		ret = fork();
		if (ret == -1)return -3;
		if (ret > 0) {
			exit(0);  // �ӽ��̵���Ϊֹ
		}

		// ����̵��������£������ػ�״̬
		for (int i = 0; i < 3; i++) {
			close(i); // �ر��������������Ϊ�˷�ֹ�����ػ����̺��������ݴ�ӡ����������
		}
		umask(0);
		signal(SIGCHLD, SIG_IGN);
		return 0;
	}
private:
	// ����̺���
	CFunctionBase* m_func;
	// �����id
	pid_t m_pid;
	// ���̼�ͨ�Źܵ�
	int m_pipes[2];
};


