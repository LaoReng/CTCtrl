import { equipmentListResData } from "@/api/home/type";
export interface HomeStoreType {
  isLoading: boolean;
  equipInfo: equipmentListResData | any;
  requestParam: {
    ctrlerPhone: string; // 控制者的手机号
    bectrlEquipName: string; // 被控设备名称
    startCtrlTime: string; // 开始控制的时间xxxx-xx-xx xx:xx:xx
    key: string;
  };
  selectedCard: any[];
  saveBusyDevice: boolean;
  isShowShutdownOrReboot: boolean;
  ShutdownOrRebootParam: any[];
  MyType: number;
  downLoadList: any[];
  isShowFileSystemDialog: boolean;
  isShowDownLoadList: boolean;
  isClickStartOrPause: boolean;
  isClickEnd: boolean;
  rightClickPreviewDeviceName: string;
}
