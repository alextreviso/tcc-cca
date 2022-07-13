profile               = "ilg-atreviso"
vpc_name              = "atreviso"
vpc_cidr              = "10.0.0.0/19"
private_subnets_cidrs = ["10.0.0.0/22", "10.0.4.0/22", "10.0.8.0/22", "10.0.12.0/22"]
public_subnets_cidrs  = ["10.0.16.0/22", "10.0.20.0/22", "10.0.24.0/22", "10.0.28.0/22"]
region                = "us-east-1"
env                   = "dev"

database_name               = "atreviso"
allowed_cidrs               = ["0.0.0.0/0"]
engine                      = "postgres"
engine_version              = "13.4"
db_username                 = "atreviso"
db_password                 = "admin1234"
db_port                     = "5432"
deletion_protection         = true
storage_type                = "gp2"
instance_class              = "db.t3.micro"
publicly_accessible         = true
allocated_storage           = "1000" 

ecr_name                    = "atreviso"

container_port      = 8080
task_memory         = 1024
task_cpu            = 512
healthcheck_url     = "/"
private_access      = false
