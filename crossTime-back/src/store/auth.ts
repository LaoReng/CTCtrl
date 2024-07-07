import { defineStore } from 'pinia';
import { reactive } from 'vue';
import { adminLogin } from '@/api/admin';
import { routes } from '@/router/routes';
import { CLEAR_TOKEN, GET_TOKEN, SET_TOKEN } from "@/utils/token";
export const useAuthStore = defineStore('auth', {
  state: () => ({
    token: GET_TOKEN(),
    menuRoutes: routes,
    userForm: reactive({
      phone: '',
      password: '',
    }),
  }),
  actions: {
    async login() {
      try {
        const response:any = await adminLogin(this.userForm.phone, this.userForm.password);
        // 登录接口的响应数据
        // console.log(response);
        if (response.status === 0) {
          SET_TOKEN(this.userForm.phone);
          this.token = this.userForm.phone;
        } else {
          alert('手机号或密码不正确');
        }
      } catch (error) {
        console.error('登录失败:', error);
      }
    },
    logout() {
      this.token = null;
      CLEAR_TOKEN();
    },
  },
});
export default useAuthStore;