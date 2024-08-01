# -*- coding: utf-8 -*-
import dataclasses


@dataclasses.dataclass
class BaseConfig:
    args: dict
    base_dir: str


GLOBAL_BASE_CONFIG = BaseConfig(args={}, base_dir='')
