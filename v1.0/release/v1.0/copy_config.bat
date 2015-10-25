@echo 拷贝配置文件
@echo Debug版本
copy client_datastore_path.txt Debug\client_datastore_path.txt
copy master_datastore_path.txt Debug\master_datastore_path.txt
copy slave_datastore_path.txt Debug\slave_datastore_path.txt

@echo Release版本
copy client_datastore_path.txt Release\client_datastore_path.txt
copy master_datastore_path.txt Release\master_datastore_path.txt
copy slave_datastore_path.txt Release\slave_datastore_path.txt
