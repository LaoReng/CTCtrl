//存储Ip

export const SET_IP = (ip: any) => {
  localStorage.setItem("ip", ip);
};
export const GET_IP = () => {
  return JSON.parse(localStorage.getItem("ip") as string);
};
