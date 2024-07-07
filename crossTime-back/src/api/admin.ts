import request from '@/utils/request';
// 管理员登录接口 
// phone:15046543174   password:admin
// phone:15945460831   password:admin
export function adminLogin(phone: string | null, password: string | null) {
  return request.get('/login', {
    params: {
      phone,
      password,
    },
  });
}