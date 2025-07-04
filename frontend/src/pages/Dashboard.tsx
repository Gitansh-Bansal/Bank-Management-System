import React, { useState, useEffect } from 'react';
import { useNavigate } from 'react-router-dom';
import {
  Container,
  Box,
  Typography,
  Button,
  Grid,
  Card,
  CardContent,
  CardActions,
  Dialog,
  DialogTitle,
  DialogContent,
  DialogActions,
  TextField,
  MenuItem,
  Chip,
  Avatar,
  IconButton,
  AppBar,
  Toolbar,
  Divider,
} from '@mui/material';
import {
  Add as AddIcon,
  AccountBalance as AccountBalanceIcon,
  Person as PersonIcon,
  Logout as LogoutIcon,
  Visibility as VisibilityIcon,
  Savings as SavingsIcon,
  AccountBalanceWallet as CurrentIcon,
  Security as AuditableIcon,
} from '@mui/icons-material';
import { useAuth } from '../contexts/AuthContext';
import axios from 'axios';

interface Account {
  accountNumber: number;
  type: string;
  balance: number;
}

const getAccountIcon = (type: string) => {
  switch (type.toLowerCase()) {
    case 'savings':
      return <SavingsIcon />;
    case 'current':
      return <CurrentIcon />;
    case 'auditable':
      return <AuditableIcon />;
    default:
      return <AccountBalanceIcon />;
  }
};

const getAccountColor = (type: string) => {
  switch (type.toLowerCase()) {
    case 'savings':
      return 'success';
    case 'current':
      return 'primary';
    case 'auditable':
      return 'warning';
    default:
      return 'default';
  }
};

const API_BASE_URL = import.meta.env.VITE_API_URL || 'http://localhost:3001';

export default function Dashboard() {
  const [accounts, setAccounts] = useState<Account[]>([]);
  const [openDialog, setOpenDialog] = useState(false);
  const [accountType, setAccountType] = useState('');
  const [initialBalance, setInitialBalance] = useState('');
  const [accountPassword, setAccountPassword] = useState('');
  const navigate = useNavigate();
  const { user, logout } = useAuth();

  useEffect(() => {
    if (!user) {
      navigate('/login');
      return;
    }
    fetchAccounts();
  }, [user, navigate]);

  const fetchAccounts = async () => {
    try {
      const response = await axios.get(`${API_BASE_URL}/api/accounts?username=${user?.username}`);
      setAccounts(response.data.accounts || []);
    } catch (error) {
      console.error('Failed to fetch accounts:', error);
      setAccounts([]);
    }
  };

  const handleCreateAccount = async () => {
    try {
      await axios.post(`${API_BASE_URL}/api/accounts`, {
        username: user?.username,
        type: accountType,
        initialBalance: parseFloat(initialBalance),
        password: accountPassword,
      });
      setOpenDialog(false);
      setAccountType('');
      setInitialBalance('');
      setAccountPassword('');
      fetchAccounts();
    } catch (error) {
      console.error('Failed to create account:', error);
    }
  };

  const handleLogout = () => {
    logout();
    navigate('/login');
  };

  const totalBalance = accounts.reduce((sum, account) => sum + account.balance, 0);

  return (
    <Box sx={{ minHeight: '100vh', bgcolor: 'background.default' }}>
      <AppBar position="static" elevation={0} sx={{ bgcolor: 'white', borderBottom: '1px solid #e2e8f0' }}>
        <Toolbar>
          <AccountBalanceIcon sx={{ color: 'primary.main', mr: 2, fontSize: 32 }} />
          <Typography variant="h6" component="div" sx={{ flexGrow: 1, color: 'text.primary', fontWeight: 600 }}>
            Sampatti Bank
          </Typography>
          <Box sx={{ display: 'flex', alignItems: 'center', gap: 2 }}>
            <Button
              startIcon={<PersonIcon />}
              variant="outlined"
              onClick={() => navigate('/profile')}
              sx={{ textTransform: 'none' }}
            >
              Profile
            </Button>
            <IconButton onClick={handleLogout} color="primary">
              <LogoutIcon />
            </IconButton>
          </Box>
        </Toolbar>
      </AppBar>

      <Container maxWidth="lg" sx={{ mt: 4, mb: 4 }}>
        <Box sx={{ mb: 4 }}>
          <Typography variant="h4" component="h1" sx={{ mb: 1, fontWeight: 700 }}>
            Welcome back, {user?.name}!
          </Typography>
          <Typography variant="body1" color="text.secondary">
            Manage your accounts and transactions securely
          </Typography>
        </Box>

        {/* Summary Card */}
        <Card sx={{ mb: 4, background: 'linear-gradient(135deg, #667eea 0%, #764ba2 100%)', color: 'white' }}>
          <CardContent sx={{ p: 3 }}>
            <Box sx={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center' }}>
              <Box>
                <Typography variant="h6" sx={{ mb: 1, opacity: 0.9 }}>
                  Total Balance
                </Typography>
                <Typography variant="h3" sx={{ fontWeight: 700 }}>
                  ${totalBalance.toFixed(2)}
                </Typography>
                <Typography variant="body2" sx={{ mt: 1, opacity: 0.8 }}>
                  Across {accounts.length} account{accounts.length !== 1 ? 's' : ''}
                </Typography>
              </Box>
              <AccountBalanceIcon sx={{ fontSize: 64, opacity: 0.3 }} />
            </Box>
          </CardContent>
        </Card>

        {/* Accounts Grid */}
        <Box sx={{ mb: 4 }}>
          <Box sx={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center', mb: 3 }}>
            <Typography variant="h5" sx={{ fontWeight: 600 }}>
              Your Accounts
            </Typography>
            <Button
              variant="contained"
              startIcon={<AddIcon />}
              onClick={() => setOpenDialog(true)}
              sx={{ textTransform: 'none' }}
            >
              New Account
            </Button>
          </Box>

          {accounts.length === 0 ? (
            <Card sx={{ textAlign: 'center', py: 6 }}>
              <AccountBalanceIcon sx={{ fontSize: 64, color: 'text.secondary', mb: 2 }} />
              <Typography variant="h6" color="text.secondary" sx={{ mb: 1 }}>
                No accounts yet
              </Typography>
              <Typography variant="body2" color="text.secondary" sx={{ mb: 3 }}>
                Create your first account to get started
              </Typography>
              <Button
                variant="contained"
                startIcon={<AddIcon />}
                onClick={() => setOpenDialog(true)}
                sx={{ textTransform: 'none' }}
              >
                Create Account
              </Button>
            </Card>
          ) : (
            <Grid container spacing={3}>
              {accounts.map((account) => (
                <Grid item xs={12} sm={6} md={4} key={account.accountNumber}>
                  <Card sx={{ height: '100%', display: 'flex', flexDirection: 'column' }}>
                    <CardContent sx={{ flexGrow: 1, p: 3 }}>
                      <Box sx={{ display: 'flex', justifyContent: 'space-between', alignItems: 'flex-start', mb: 2 }}>
                        <Avatar
                          sx={{
                            bgcolor: `${getAccountColor(account.type)}.main`,
                            width: 48,
                            height: 48,
                          }}
                        >
                          {getAccountIcon(account.type)}
                        </Avatar>
                        <Chip
                          label={account.type}
                          color={getAccountColor(account.type) as any}
                          size="small"
                          variant="outlined"
                        />
                      </Box>
                      
                      <Typography variant="h6" sx={{ mb: 1, fontWeight: 600 }}>
                        Account #{account.accountNumber}
                      </Typography>
                      
                      <Typography variant="h4" sx={{ fontWeight: 700, color: 'primary.main', mb: 1 }}>
                        ${account.balance.toFixed(2)}
                      </Typography>
                      
                      <Typography variant="body2" color="text.secondary">
                        Available balance
                      </Typography>
                    </CardContent>
                    
                    <Divider />
                    
                    <CardActions sx={{ p: 2 }}>
                      <Button
                        fullWidth
                        variant="outlined"
                        startIcon={<VisibilityIcon />}
                        onClick={() => navigate(`/account/${account.accountNumber}`)}
                        sx={{ textTransform: 'none' }}
                      >
                        View Details
                      </Button>
                    </CardActions>
                  </Card>
                </Grid>
              ))}
            </Grid>
          )}
        </Box>

        {/* Create Account Dialog */}
        <Dialog open={openDialog} onClose={() => setOpenDialog(false)} maxWidth="sm" fullWidth>
          <DialogTitle sx={{ pb: 1 }}>
            <Typography variant="h6" sx={{ fontWeight: 600 }}>
              Create New Account
            </Typography>
          </DialogTitle>
          <DialogContent>
            <TextField
              select
              fullWidth
              label="Account Type"
              value={accountType}
              onChange={(e) => setAccountType(e.target.value)}
              margin="normal"
              sx={{ mb: 2 }}
            >
              <MenuItem value="savings">Savings Account</MenuItem>
              <MenuItem value="current">Current Account</MenuItem>
              <MenuItem value="auditable">Auditable Savings</MenuItem>
            </TextField>
            <TextField
              fullWidth
              label="Initial Balance"
              type="number"
              value={initialBalance}
              onChange={(e) => setInitialBalance(e.target.value)}
              margin="normal"
              sx={{ mb: 2 }}
              InputProps={{
                startAdornment: <Typography sx={{ mr: 1 }}>$</Typography>,
              }}
            />
            <TextField
              fullWidth
              label="Account Password"
              type="password"
              value={accountPassword}
              onChange={(e) => setAccountPassword(e.target.value)}
              margin="normal"
              helperText="This password will be required for account transactions"
            />
          </DialogContent>
          <DialogActions sx={{ p: 3, pt: 1 }}>
            <Button onClick={() => setOpenDialog(false)} sx={{ textTransform: 'none' }}>
              Cancel
            </Button>
            <Button
              onClick={handleCreateAccount}
              variant="contained"
              sx={{ textTransform: 'none' }}
              disabled={!accountType || !initialBalance || !accountPassword}
            >
              Create Account
            </Button>
          </DialogActions>
        </Dialog>
      </Container>
    </Box>
  );
} 