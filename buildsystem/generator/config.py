# -*- coding: utf-8 -*-
from typing import Optional

from pydantic import BaseModel, Field


class BaseConfig(BaseModel):
    args: Optional[dict] = Field({})
    unknown_args: Optional[dict] = Field({})
    base_dir: str


GLOBAL_BASE_CONFIG = BaseConfig(base_dir='')
