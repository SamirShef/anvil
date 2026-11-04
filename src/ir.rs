use std::collections::HashMap;

pub type InstId = usize;
pub type BasicBlockId = usize;
pub type FunctionId = usize;

pub enum Inst {
    Binary(InstId, InstId),
    Call(FunctionId),
    Jump(BasicBlockId),
    JumpIf(InstId, BasicBlockId, BasicBlockId),
    Alloca,
    Load(InstId),
    Store(InstId),
}

pub struct BasicBlock {
    pub body: Vec<Inst>,
}

pub struct Function {
    pub body: Vec<BasicBlock>,
}

pub enum NameVariant {
    Name(String),
    Id(usize),
}

pub struct Context {
    funcs: Vec<Function>,

    inst_names: HashMap<InstId, NameVariant>,
    bb_names: HashMap<BasicBlockId, NameVariant>,
    funcs_names: HashMap<FunctionId, NameVariant>,
}
