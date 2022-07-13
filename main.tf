terraform {
  required_version = ">= 1.0.0"
  required_providers {
    aws = ">= 3"
  }

  cloud {
    organization = "atreviso"
    workspaces {
      name = "tcc-cca"
    }
  }
}

provider "aws" {
  region = var.region
}

module "vpc" {
  source                = "app.terraform.io/atreviso/vpc/aws"
  version               = "1.0.0"
  vpc_name              = var.vpc_name
  vpc_cidr              = var.vpc_cidr
  public_subnets_cidrs  = var.public_subnets_cidrs
  private_subnets_cidrs = var.private_subnets_cidrs
  region                = var.region
  env                   = var.env
  private_subnets       = module.vpc.private_subnets
  public_subnets        = module.vpc.public_subnets
  internet_gateway      = module.vpc.internet_gateway
}

# module "rds" {
#   source              = "app.terraform.io/atreviso/rds/aws"
#   version             = "1.0.0"
#   database_name       = var.database_name
#   allowed_cidrs       = var.allowed_cidrs
#   engine              = var.engine
#   engine_version      = var.engine_version
#   db_username         = var.db_username
#   db_password         = var.db_password
#   db_port             = var.db_port
#   deletion_protection = var.deletion_protection
#   vpc_id              = module.vpc.vpc_id
#   private_subnets_ids = module.vpc.public_subnets
#   storage_type        = var.storage_type
#   allocated_storage   = var.allocated_storage
#   instance_class      = var.instance_class
#   publicly_accessible = var.publicly_accessible
#   region              = var.region
#   env                 = var.env
# }

module "ecr" {
  source   = "app.terraform.io/atreviso/ecr/aws"
  version  = "1.0.1"
  ecr_name = var.ecr_name
  region   = var.region
  env      = var.env
}

module "ecs" {
  source              = "app.terraform.io/atreviso/ecs/aws"
  version             = "1.0.1"
  app_name            = var.app_name
  env                 = var.env
  region              = var.region
  vpc_id              = module.vpc.vpc_id
  public_subnets_ids  = module.vpc.public_subnets
  private_subnets_ids = module.vpc.private_subnets
  container_port      = var.container_port
  task_memory         = var.task_memory
  task_cpu            = var.task_cpu
  allowed_cidrs       = var.allowed_cidrs
  healthcheck_url     = var.healthcheck_url
  private_access      = var.private_access
  repository_url      = module.ecr.repository_url
}