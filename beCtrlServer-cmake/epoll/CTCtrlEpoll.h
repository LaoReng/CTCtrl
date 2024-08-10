#pragma once
#include <cstdio>
#include <unistd.h>
#include <sys/epoll.h>

// TODO:��EPOLL���в�����ʱ���ǲ�����Ҫ�����أ���
class CEpoll
{
public:
	/// <summary>
	/// ���캯��
	/// </summary>
	/// <param name="size">����epoll�����ݵ�size��αֵ</param>
	CEpoll(int size = 1);
	/// <summary>
	/// ��������
	/// </summary>
	~CEpoll();
	/// <summary>
	/// ��ʼ�������Բ����á�
	/// </summary>
	/// <param name="size">����epoll�����ݵ�size��αֵ</param>
	/// <returns>��ʼ���ɹ����򷵻�ֵΪ0����ʼ��ʧ�ܣ�����ֵΪ-1
	/// errno�鿴������   strerror�鿴������Ϣ
	/// </returns>
	int Init(int size = 1);
	/// <summary>
	/// ��epoll����ļ�������
	/// </summary>
	/// <param name="fd">Ҫ��ӵ�epoll���ļ�������</param>
	/// <param name="ev">���ļ���������Ҫ�������¼���Я��������</param>
	/// <returns>��ӳɹ����򷵻�ֵΪ0�����ʧ�ܣ�����ֵΪ-1
	/// errno�鿴������   strerror�鿴������Ϣ
	/// </returns>
	int Add_fd(int fd, struct epoll_event* ev);
	/// <summary>
	/// �޸�epoll�д��ڵ��ļ�������
	/// </summary>
	/// <param name="fd">Ҫ�޸ĵ�epoll���ļ�������</param>
	/// <param name="ev">���ļ���������Ҫ�������¼���Я��������</param>
	/// <returns>�޸ĳɹ����򷵻�ֵΪ0���޸�ʧ�ܣ�����ֵΪ-1
	/// errno�鿴������   strerror�鿴������Ϣ
	/// </returns>
	int Mod_fd(int fd, struct epoll_event* ev);
	/// <summary>
	/// ���ļ���������epollɾ��
	/// </summary>
	/// <param name="fd">Ҫɾ�����ļ�������</param>
	/// <returns>ɾ���ɹ����򷵻�ֵΪ0��ɾ��ʧ�ܣ�����ֵΪ-1
	/// errno�鿴������   strerror�鿴������Ϣ
	/// </returns>
	int Del_fd(int fd);
	/// <summary>
	/// �ȴ�epoll�����¼�����
	/// </summary>
	/// <param name="evlist">epoll_event�ṹ�����飬�����йؾ���̬�ļ�����������Ϣ</param>
	/// <param name="Maxevents">ָ��evlist�����п��԰��������Ԫ�ظ���</param>
	/// <param name="timeout">���õȴ�������Ϊ��-1��ʾһֱ������0��ʾִ��һ�η������ؼ�飻����0��ʾ��������timeout���룬�����¼�����ʱ����ǰ����</param>
	/// <returns>����ֵ��ʾevlist������Ԫ�صĸ��� ���ִ����򷵻�ֵС��0  errno�鿴������   strerror�鿴������Ϣ</returns>
	int EWait(struct epoll_event* evlist, int Maxevents, int timeout);
	/// <summary>
	/// �ر�epoll�׽���
	/// </summary>
	/// <returns></returns>
	int EClose();
private:
	int m_epfd;
};

